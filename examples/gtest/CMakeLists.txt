# Copyright (c) 2020  ETH Zurich
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.13)

project(gtest CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
include(HPX_SetupGoogleTest)

if(EXISTS "${HPX_DIR}")

  find_package(HPX REQUIRED)

  # Main in the same file as the hpx/hpx_main.hpp include
  add_executable(gtest_with_main gtest_with_main.cpp)
  target_link_libraries(gtest_with_main PRIVATE gtest)
  target_link_libraries(gtest_with_main PRIVATE HPX::hpx HPX::wrap_main)

  # Main in a separate static library (in gtest_main) using hpx_wrap to start
  # the runtime
  add_executable(gtest_hpx_start gtest_hpx_start.cpp)
  target_link_libraries(gtest_hpx_start PRIVATE HPX::hpx)
  target_link_libraries(gtest_hpx_start PRIVATE gtest_main)

  # Main in a separate static library (in gtest_main) using hpx/hpx_main.hpp
  add_executable(gtest_minimal gtest_minimal.cpp)

  # Need to have an interface library which links those before linking to the
  # executable otherwise the order is not respected
  add_library(gtest_main_hpx_interface INTERFACE)
  target_link_libraries(
    gtest_main_hpx_interface INTERFACE HPX::hpx HPX::wrap_main gtest_main
  )

  target_link_libraries(gtest_minimal PRIVATE gtest_main_hpx_interface)

else()
  message(
    WARNING
      "HPX_DIR=${HPX_DIR} does not exist. Did you forget to run the install rule?"
  )
endif()
