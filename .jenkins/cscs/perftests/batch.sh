#!/bin/bash -l

# Copyright (c) 2020 ETH Zurich
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

# Args for the pyutils suite
logfile=jenkins-hpx-${configuration_name}.out

orig_src_dir="$(pwd)"
src_dir="/dev/shm/hpx/src"
build_dir="/dev/shm/hpx/build"

# Copy source directory to /dev/shm for faster builds
mkdir -p "${build_dir}"
cp -r "${orig_src_dir}" "${src_dir}"
envfile=${src_dir}/.jenkins/cscs/env-${configuration_name}.sh
# Copy the perftest utility in the build dir
mkdir -p ${build_dir}/tools/perftests_ci
cp -r ${src_dir}/tools/perftests_ci/* ${build_dir}/tools/perftests_ci

# Variables
perftests_dir=${build_dir}/tools/perftests_ci
result_dir=${build_dir}/tools/perftests_ci/results
mkdir -p $result_dir
result=$result_dir/local-priority-fifo.json

# FIXME: we can probably do this step as the other tests and use gridtools
# script only for running and plotting
# Build binaries for performance tests
${perftests_dir}/driver.py -v -l $logfile build -b release \
    -o build --source-dir ${src_dir} --build-dir ${build_dir} -e $envfile \
    -t tests.performance.local.future_overhead_report \
    || { echo 'Build failed'; exit 1; }

# TODO: make schedulers and other options vary
#for domain in 128 256; do

  # Run performance tests
  ${perftests_dir}/driver.py -v -l $logfile perftest run \
      --local True --scheduling-policy local-priority --run_output $result \
      --extra-opts ' --test-all --repetitions=15' \
      || { echo 'Running failed'; exit 1; }
  # We add a space before --test-all because of the following issue
  # https://bugs.python.org/issue9334

  # Create directory for reports
  mkdir -p ${build_dir}/reports
  # Find references for same configuration (TODO: specify for scheduler etc.)
  reference=${perftests_dir}/perftest/references/daint_default/local-priority-fifo.json

  # Plot comparison of current result with references
  ${perftests_dir}/driver.py -v -l $logfile perftest plot compare \
      -i $reference $result -o ${build_dir}/reports/reports-comparison \
      || { echo 'Plotting failed'; exit 1; }
#done

# Copy the testing directory for saving as an artifact
cp -r ${build_dir}/Testing ${orig_src_dir}/${configuration_name}-Testing
cp -r ${build_dir}/reports ${orig_src_dir}/${configuration_name}-reports

# Things went wrong by default
ctest_exit_code=$?
file_errors=1
configure_errors=1
build_errors=1
test_errors=1
# Temporary as the output files have not been set up
if [[ -f ${build_dir}/Testing/TAG ]]; then
    file_errors=0
    tag="$(head -n 1 ${build_dir}/Testing/TAG)"

    if [[ -f "${build_dir}/Testing/${tag}/Configure.xml" ]]; then
        configure_errors=$(grep '<Error>' "${build_dir}/Testing/${tag}/Configure.xml" | wc -l)
    fi

    if [[ -f "${build_dir}/Testing/${tag}/Build.xml" ]]; then
        build_errors=$(grep '<Error>' "${build_dir}/Testing/${tag}/Build.xml" | wc -l)
    fi

    if [[ -f "${build_dir}/Testing/${tag}/Test.xml" ]]; then
        test_errors=$(grep '<Test Status=\"failed\">' "${build_dir}/Testing/${tag}/Test.xml" | wc -l)
    fi
fi
ctest_status=$(( ctest_exit_code + file_errors + configure_errors + build_errors + test_errors ))

echo "${ctest_status}" > "jenkins-hpx-${configuration_name}-ctest-status.txt"
exit $ctest_status
