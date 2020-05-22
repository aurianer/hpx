//  Copyright (c) 2020 ETH Zurich
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#if defined(HPX_HAVE_CUDA)

    #include <cuda_runtime.h>

#elif defined(HPX_HAVE_HIP)

    #include <hip/hip_runtime.h>

    #define cudaError_t hipError_t
    #define cudaGetErrorString hipGetErrorString
    #define cudaMalloc hipMalloc
    #define cudaStreamAddCallback hipStreamAddCallback
    #define cudaStream_t hipStream_t
    #define cudaSuccess hipSuccess

#endif
