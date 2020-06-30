//  Copyright (c) 2020 ETH Zurich
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include <hpx/hpx_main.hpp>

TEST(HPXGoogleTest, GTestAssert)
{
    if (::hpx::threads::get_self_ptr() == nullptr)
    {
        ASSERT_EQ(8.0, 4.0);
    }
    else
    {
        ASSERT_EQ(8.0, 8.0);
    }
}
