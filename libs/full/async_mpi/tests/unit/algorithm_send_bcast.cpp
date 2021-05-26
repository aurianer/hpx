//  Copyright (c) 2021 ETH Zurich
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/modules/async_mpi.hpp>
#include <hpx/modules/execution.hpp>
#include <hpx/modules/testing.hpp>

#include <mpi.h>

namespace ex = hpx::execution::experimental;
namespace mpi = hpx::mpi::experimental;

template <typename F>
struct callback_receiver
{
    std::decay_t<F> f;
    std::atomic<bool>& set_value_called;

    template <typename E>
    void set_error(E&&) noexcept
    {
        HPX_TEST(false);
    }

    void set_done() noexcept
    {
        HPX_TEST(false);
    };

    template <typename... Ts>
    auto set_value(Ts&&... ts) noexcept
        -> decltype(HPX_INVOKE(f, std::forward<Ts>(ts)...), void())
    {
        HPX_INVOKE(f, std::forward<Ts>(ts)...);
        set_value_called = true;
    }
};

//template <typename F>
//struct error_callback_receiver
//{
//    std::decay_t<F> f;
//    std::atomic<bool>& set_error_called;
//
//    template <typename E>
//    void set_error(E&& e) noexcept
//    {
//        HPX_INVOKE(f, std::forward<E>(e));
//        set_error_called = true;
//    }
//
//    void set_done() noexcept
//    {
//        HPX_TEST(false);
//    };
//
//    template <typename... Ts>
//    void set_value(Ts&&...) noexcept
//    {
//        HPX_TEST(false);
//    }
//};

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);
    int size, rank;
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);

    HPX_TEST_MSG(size > 1, "This test requires N>1 mpi ranks");

    MPI_Datatype datatype = MPI_INT;

    {
        int data, count = 1;
        std::atomic<bool> set_value_called{false};
        auto s = mpi::send_bcast(ex::just(&data, count, datatype, 0, comm));
        auto f = [] {};
        auto r = callback_receiver<decltype(f)>{f, set_value_called};
        auto os = ex::connect(std::move(s), std::move(r));
        ex::start(os);
        HPX_TEST(set_value_called);
    }

    {
        int data = 0, count = 1;
        if (rank == 0)
        {
            data = 42;
        }
        std::atomic<bool> set_value_called{false};
        auto s = mpi::send_bcast(ex::when_all(ex::just(&data), ex::just(count),
                ex::just(datatype), ex::just(0), ex::just(comm)));
        auto f = [rank, &data]() {
            if (rank != 0)
            {
                HPX_TEST_EQ(data, 42);
            }
        };
        auto r = callback_receiver<decltype(f)>{f, set_value_called};
        auto os = ex::connect(std::move(s), std::move(r));
        ex::start(os);
        HPX_TEST(set_value_called);
    }

    MPI_Finalize();

    return hpx::util::report_errors();
}
