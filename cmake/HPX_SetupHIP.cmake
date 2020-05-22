# Copyright (c)      2020 ETH Zurich
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

if(HPX_WITH_HIP)
  if(HPX_WITH_CUDA)
    hpx_warn("Both HPX_WITH_CUDA and HPX_WITH_HIP are ON. Please choose one of \
    them for HPX to work properly")
  endif(HPX_WITH_CUDA)

  if(NOT DEFINED HIP_PATH)
      if(NOT DEFINED ENV{HIP_PATH})
          set(HIP_PATH "/opt/rocm/hip" CACHE PATH "Path to which HIP has been installed")
      else()
          set(HIP_PATH $ENV{HIP_PATH} CACHE PATH "Path to which HIP has been installed")
      endif()
  endif()
  set(CMAKE_MODULE_PATH "${HIP_PATH}/cmake" ${CMAKE_MODULE_PATH})

  find_package(HIP QUIET)
  if(HIP_FOUND)
      hpx_info("Found HIP: " ${HIP_VERSION})
  else()
      hpx_error("Could not find HIP. Ensure that HIP is either installed in \
      /opt/rocm/hip or the variable HIP_PATH is set to point to the right \
      location.")
  endif()

  # To complete with supported cuda architecture
  if (NOT GPU_ARCH)
    set(GPU_ARCH Mi50)
  endif(NOT GPU_ARCH)
  set(GPU_ARCH_NUMBER_Mi50 gfx906)

  hpx_info("  Compiling for platform: " ${HIP_PLATFORM})

  # set appropriate compilation flags depending on platform
  set(GPU_ARCH_NUMBER ${GPU_ARCH_NUMBER_${GPU_ARCH}})
  set(HIP_HIPCC_FLAGS "${HIP_HIPCC_FLAGS} -D__HIP -O3")
  if (${HIP_PLATFORM} STREQUAL "nvcc")
      # Not tested yet
      #set(HIP_HIPCC_FLAGS "${HIP_HIPCC_FLAGS} -std=c++11 \
      #-arch=sm_${GPU_ARCH_NUMBER} --cudart static -nogpulib")
  else()
    set(HIP_HIPCC_FLAGS "${HIP_HIPCC_FLAGS} --amdgpu-target=${GPU_ARCH_NUMBER} \
    -fPIC -std=c++${HPX_CXX_STANDARD}")
  endif()
  hpx_info("  GPU target architecture: " ${GPU_ARCH})
  hpx_info("  GPU architecture number: " ${GPU_ARCH_NUMBER})
  hpx_info("  HIPCC flags: " ${HIP_HIPCC_FLAGS})

  set(HPX_WITH_COMPUTE ON)
  hpx_add_config_define(HPX_HAVE_COMPUTE)
  hpx_add_config_define(HPX_HAVE_HIP)

endif(HPX_WITH_HIP)
