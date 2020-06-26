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

    #define cudaDeviceProp hipDeviceProp
    #define cudaError_t hipError_t
    #define cudaFree hipFree
    #define cudaGetDevice hipGetDevice
    #define cudaGetDeviceCount hipGetDeviceCount
    #define cudaGetDeviceProperties hipGetDeviceProperties
    #define cudaGetErrorString hipGetErrorString
    #define cudaGetLastError hipGetLastError
    #define cudaGetParameterBuffer hipGetParameterBuffer
    #define cudaLaunchDevice hipLaunchDevice
    #define cudaLaunchKernel hipLaunchKernel
    #define cudaMalloc hipMalloc
    #define cudaMemcpy hipMemcpy
    #define cudaMemcpyAsync hipMemcpyAsync
    #define cudaMemcpyDeviceToDevice hipMemcpyDeviceToDevice
    #define cudaMemcpyDeviceToHost hipMemcpyDeviceToHost
    #define cudaMemcpyHostToDevice hipMemcpyHostToDevice
    #define cudaMemGetInfo hipMemGetInfo
    #define cudaMemsetAsync hipMemsetAsync
    #define cudaSetDevice hipSetDevice
    #define cudaStream_t hipStream_t
    #define cudaStreamAddCallback hipStreamAddCallback
    #define cudaStreamCreate hipStreamCreate
    #define cudaStreamCreateWithFlags hipStreamCreateWithFlags
    #define cudaStreamDestroy hipStreamDestroy
    #define cudaStreamNonBlocking hipStreamNonBlocking
    #define cudaStreamSynchronize hipStreamSynchronize
    #define cudaSuccess hipSuccess

#endif
