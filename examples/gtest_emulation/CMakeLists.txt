# Copyright (c) 2020  ETH Zurich
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.13)

project(gtest CXX)

if(EXISTS "${HPX_DIR}")

  find_package(HPX REQUIRED)

  # Add a static library which contains a main to emulate gtest_main
  add_library(static_main_lib STATIC static_main.cpp)

  # /!\ This helper interface is needed to keep the right linking order
  add_library(hpx_helper_interface INTERFACE)
  target_link_libraries(
    hpx_helper_interface INTERFACE HPX::hpx HPX::wrap_main static_main_lib
  )

  # Test with the main function in a separate static library
  add_executable(hpx_main_ext_main hpx_main_ext_main.cpp)
  target_link_libraries(hpx_main_ext_main PRIVATE hpx_helper_interface)

  # Use of hpx_start so that it doesn't break at hpx/hpx_main.hpp not found but
  # at the undefined reference to main
  add_executable(hpx_start_ext_main hpx_start_ext_main.cpp)
  target_link_libraries(hpx_start_ext_main PRIVATE HPX::hpx static_main_lib)

else()
  message(
    WARNING
      "HPX_DIR=${HPX_DIR} does not exist. Did you forget to run the install rule?"
  )
endif()
