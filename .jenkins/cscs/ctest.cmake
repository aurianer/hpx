#  Copyright (c) 2020 ETH Zurich
#  Copyright (c) 2017 John Biddiscombe
#
#  Distributed under the Boost Software License, Version 1.0. (See accompanying
#  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

set(CTEST_TEST_TIMEOUT 120)
set(CTEST_BUILD_PARALLELISM 20)
set(CTEST_TEST_PARALLELISM 8)
set(CTEST_CMAKE_GENERATOR Ninja)
set(CTEST_SITE "cscs(daint)")
set(CTEST_BUILD_NAME "$ENV{ghprbPullId}-${CTEST_BUILD_CONFIGURATION_NAME}")
set(CTEST_GIT_COMMAND "git")
set(CTEST_GIT_UPDATE_CUSTOM "exit 0")

message("CTEST_BUILD_NAME=${CTEST_BUILD_NAME}")

set(CTEST_CONFIGURE_COMMAND "${CMAKE_COMMAND} ${CTEST_SOURCE_DIRECTORY}")
set(CTEST_CONFIGURE_COMMAND "${CTEST_CONFIGURE_COMMAND} -G${CTEST_CMAKE_GENERATOR}")
set(CTEST_CONFIGURE_COMMAND "${CTEST_CONFIGURE_COMMAND} -B${CTEST_BINARY_DIRECTORY}")
set(CTEST_CONFIGURE_COMMAND "${CTEST_CONFIGURE_COMMAND} ${CTEST_CONFIGURE_EXTRA_OPTIONS}")

ctest_start(Experimental TRACK "Pull_Requests")
ctest_update()
ctest_submit(PARTS Update BUILD_ID CTEST_BUILD_ID)
file(WRITE "jenkins-hpx-$ENV{configuration_name}-cdash-build-id.txt" "${CTEST_BUILD_ID}")
ctest_configure()
ctest_submit(PARTS Configure)
ctest_build(
  TARGET all
  FLAGS "-k0 -j ${CTEST_BUILD_PARALLELISM}")
ctest_build(
  TARGET tests
  FLAGS "-k0 -j ${CTEST_BUILD_PARALLELISM}")
ctest_submit(PARTS Build)
ctest_test(PARALLEL_LEVEL "${CTEST_TEST_PARALLELISM}")
ctest_submit(PARTS Test)
