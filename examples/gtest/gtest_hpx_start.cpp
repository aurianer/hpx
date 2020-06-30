//  Copyright (c) 2020 ETH Zurich
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx_start.hpp>
#include <hpx/include/apply.hpp>
#include <hpx/runtime/threads/thread_data.hpp>
#include <hpx/runtime_local/run_as_hpx_thread.hpp>

#include <gtest/gtest.h>

void hpx_assert_func()
{
    if (hpx::threads::get_self_ptr() == nullptr)
    {
        ASSERT_EQ(8.0, 4.0);
    }
    else
    {
        ASSERT_EQ(8.0, 8.0);
    }
}

TEST(HPXGoogleTest, GTestAssert)
{
    int argc;
    char** argv;
    ::hpx::start(nullptr, argc, argv);
    ::hpx::threads::run_as_hpx_thread(&hpx_assert_func);
    ::hpx::apply([]() { hpx::finalize(); });
    ::hpx::stop();
}
