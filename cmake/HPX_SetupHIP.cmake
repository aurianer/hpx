# Copyright (c)      2020 ETH Zurich
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

# No need for the user to specify the option explicitly
if ("${CMAKE_CXX_COMPILER}" MATCHES "hipcc$")
  include(HPX_Option)
  hpx_option(
    HPX_WITH_HIP BOOL "Enable compilation with HIPCC (default: ON)" ON
    ADVANCED
  )
else()
  hpx_option(
    HPX_WITH_HIP BOOL "Enable compilation with HIPCC (default: OFF)" OFF
    ADVANCED
  )
endif()

if(HPX_WITH_HIP)

  if(HPX_WITH_CUDA)
    hpx_error("Both HPX_WITH_CUDA and HPX_WITH_HIP are ON. Please choose one of \
    them for HPX to work properly")
  endif(HPX_WITH_CUDA)

#  # NOT Needed anymore since we set the env variable, will maybe be used again
#  #in the future
#  # To complete with supported cuda architecture
#  if (NOT GPU_ARCH)
#    set(GPU_ARCH Mi50)
#  endif(NOT GPU_ARCH)
#  set(GPU_ARCH_NUMBER_Mi50 gfx906)
#
#  hpx_info("HIP settings:")
#
#  # set appropriate compilation flags depending on platform
#  set(GPU_ARCH_NUMBER ${GPU_ARCH_NUMBER_${GPU_ARCH}})
#  if ("${HIP_PLATFORM}" STREQUAL "nvcc")
#    # Not tested yet
#    set(HIP_HIPCC_FLAGS "${HIP_HIPCC_FLAGS} -std=c++11 \
#    -arch=sm_${GPU_ARCH_NUMBER} --cudart static -nogpulib")
#  else()
#    set(HIP_HIPCC_FLAGS "${HIP_HIPCC_FLAGS} --amdgpu-target=${GPU_ARCH_NUMBER}")
#    set(HIP_HIPCC_FLAGS "${HIP_HIPCC_FLAGS} -std=c++${HPX_CXX_STANDARD}")
#  endif()
#  hpx_info("  GPU target architecture: " ${GPU_ARCH})
#  hpx_info("  GPU architecture number: " ${GPU_ARCH_NUMBER})
#  hpx_info("  HIPCC flags: " ${HIP_HIPCC_FLAGS})
#
#  add_library(HIP::hip INTERFACE IMPORTED)
#  target_compile_options(HIP::hip INTERFACE -xhip ${HIP_HIPCC_FLAGS})

  set(HPX_WITH_COMPUTE ON)
  hpx_add_config_define(HPX_HAVE_COMPUTE)
  hpx_add_config_define(HPX_HAVE_HIP)

endif(HPX_WITH_HIP)
