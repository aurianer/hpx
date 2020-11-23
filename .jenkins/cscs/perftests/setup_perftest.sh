#!/bin/bash -l

# Copyright (c) 2020 ETH Zurich
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

# Clean up old artifacts
rm -f ./jenkins-hpx* ./*-Testing

source .jenkins/cscs/slurm-constraint-${configuration_name}.sh

if [[ -z "${ghprbPullId:-}" ]]; then
    # Set name of branch if not building a pull request
    export git_local_branch=$(echo ${GIT_BRANCH} | cut -f2 -d'/')
    job_name="jenkins-hpx-${git_local_branch}-${configuration_name}"
else
    job_name="jenkins-hpx-${ghprbPullId}-${configuration_name}"

    # Cancel currently running builds on the same branch, but only for pull
    # requests
    scancel --jobname="${job_name}"
fi

# Args for the pyutils suite
logfile=jenkins-hpx-${configuration_name}.out

orig_src_dir="$(pwd)"
src_dir="/dev/shm/hpx/src"
build_dir="/dev/shm/hpx/build"

# Copy source directory to /dev/shm for faster builds
mkdir -p "${build_dir}"
cp -r "${orig_src_dir}" "${src_dir}"

source ${src_dir}/.jenkins/cscs/env-${configuration_name}.sh

# build binaries for performance tests
${src_dir}/tools/perftests_ci/driver.py -v -l $logfile build -b release \
    -o build --source-dir ${src_dir} --build-dir ${build_dir} \
    -t tests.performance.local.future_overhead \
    || { echo 'Build failed'; rm -rf $tmpdir; exit 1; }

# TODO: make schedulers and other options vary
#for domain in 128 256; do
  # result directory, create if it does not exist yet
  resultdir=${build_dir}/tools/perftests_ci/results/${configuration_name}
  mkdir -p $resultdir
  result=$resultdir/$domain.json

  # run performance tests
  ${build_dir}/tools/perftests/driver.py -v -l $logfile perftest run -o $result \
      || { echo 'Running failed'; rm -rf $tmpdir; exit 1; }

  # create directory for reports
  mkdir reports
  # find references for same configuration
  reference=./pyutils/perftest/references/${configuration}/local-priority-fifo.json
  # plot comparison of current result with references
  #./build/pyutils/driver.py -v -l $logfile perftest plot compare -i $reference $result -o reports/reference-comparison-$domain || { echo 'Plotting failed'; rm -rf $tmpdir; exit 1; }
  # plot comparison between backends
  #./build/pyutils/driver.py -v -l $logfile perftest plot compare-backends -i $result -o reports/backends-comparison-$domain || { echo 'Plotting failed'; rm -rf $tmpdir; exit 1; }
#done

# Copy the testing directory for saving as an artifact
cp -r ${build_dir}/Testing ${orig_src_dir}/${configuration_name}-Testing

# Things went wrong by default
#ctest_exit_code=$?
#file_errors=1
#configure_errors=1
#build_errors=1
#test_errors=1
# Temporary as the output files have not been set up
ctest_exit_code=0
file_errors=0
configure_errors=0
build_errors=0
test_errors=0
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

# Print slurm logs
echo "= stdout =================================================="
cat jenkins-hpx-${configuration_name}.out

# Get build status
if [[ "$(cat jenkins-hpx-${configuration_name}-ctest-status.txt)" -eq "0" ]]; then
    github_commit_status="success"
else
    github_commit_status="failure"
fi

if [[ -n "${ghprbPullId:-}" ]]; then
    # Extract just the organization and repo names "org/repo" from the full URL
    github_commit_repo="$(echo $ghprbPullLink | sed -n 's/https:\/\/github.com\/\(.*\)\/pull\/[0-9]*/\1/p')"

    # Get the CDash dashboard build id
    cdash_build_id="$(cat jenkins-hpx-${configuration_name}-cdash-build-id.txt)"

    # Extract actual token from GITHUB_TOKEN (in the form "username:token")
    github_token=$(echo ${GITHUB_TOKEN} | cut -f2 -d':')

    # Set GitHub status with CDash url
    .jenkins/common/set_github_status.sh \
        "${github_token}" \
        "${github_commit_repo}" \
        "${ghprbActualCommit}" \
        "${github_commit_status}" \
        "${configuration_name}" \
        "${cdash_build_id}" \
        "jenkins/cscs/perftests"
fi

set -e
exit $(cat jenkins-hpx-${configuration_name}-ctest-status.txt) && $ctest_status
