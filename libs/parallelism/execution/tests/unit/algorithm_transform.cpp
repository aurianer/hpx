//  Copyright (c) 2021 ETH Zurich
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//#include <hpx/hpx_main.hpp>
#include <hpx/modules/execution.hpp>
#include <hpx/modules/testing.hpp>

#include <hpx/execution/tests/algorithm_test_utils.hpp>

#include <atomic>
#include <exception>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace ex = hpx::execution::experimental;

int main()
{
    {
        std::atomic<bool> set_value_called{false};
        auto s = ex::transform(ex::just(0), [](int x) { return ++x; });
        //ex::sync_wait(std::move(s));
        ex::sync_wait(s);
        HPX_TEST(set_value_called);
    }

    return hpx::util::report_errors();
}
