//  Copyright (c) 2020 ETH Zurich
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx_start.hpp>
#include <hpx/include/apply.hpp>
#include <hpx/runtime/threads/thread_data.hpp>
#include <hpx/runtime_local/run_as_hpx_thread.hpp>

#include <hpx/modules/testing.hpp>

int check_runtime_started()
{
    // Check that HPX runtime is started
    HPX_TEST(hpx::threads::get_self_ptr() != nullptr);
    return ::hpx::util::report_errors();
}

int report_errors_hpx()
{
    int argc = 0;
    char** argv = nullptr;
    ::hpx::start(nullptr, argc, argv);
    int report_errors =
        ::hpx::threads::run_as_hpx_thread(&check_runtime_started);
    ::hpx::apply([]() { ::hpx::finalize(); });
    ::hpx::stop();
    return report_errors;
}
