//  Copyright (c) 2007-2017 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/modules/async_cuda.hpp>
#include <hpx/modules/testing.hpp>

__global__ void hipcopy(int* in, int* out) {
    *out = *in;
}

void test_target_instantiation()
{
    hpx::cuda::experimental::target target;
}

int hpx_main(int argc, char* argv[])
{
    test_target_instantiation();

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    int *in, *out;
    hipMalloc((void**)&in, sizeof(int));
    hipMalloc((void**)&out, sizeof(int));
    hipcopy<<<1,1>>>(in, out);
    hipFree(in);
    hipFree(out);
    // Initialize and run HPX
    HPX_TEST_EQ_MSG(hpx::init(argc, argv), 0,
        "HPX main exited with non-zero status");

    return hpx::util::report_errors();
}
