//  Copyright (c) 2021 ETH Zurich
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx_init.hpp>

#include <hpx/modules/async_mpi.hpp>
#include <hpx/modules/execution.hpp>
#include <hpx/modules/testing.hpp>

#include <hpx/execution/tests/algorithm_test_utils.hpp>

#include <mpi.h>

namespace ex = hpx::execution::experimental;
namespace mpi = hpx::mpi::experimental;

int hpx_main()
{
    int size, rank;
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);

    HPX_TEST_MSG(size > 1, "This test requires N>1 mpi ranks");

    MPI_Datatype datatype = MPI_INT;

    {
        mpi::enable_user_polling enable_polling;
        // Success path
        {
            int data = 0, count = 1;
            if (rank == 0)
            {
                data = 42;
            }
            auto s = mpi::send_bcast(ex::just(&data, count, datatype, 0, comm));
            ex::sync_wait(s);
            if (rank != 0)
            {
                HPX_TEST_EQ(data, 42);
            }
        }

        // Failure path
        {
            bool exception_thrown = false;
            try
            {
                mpi::send_bcast(error_sender{}) | ex::sync_wait();
                HPX_TEST(false);
            }
            catch (std::runtime_error const& e)
            {
                HPX_TEST_EQ(std::string(e.what()), std::string("error"));
                exception_thrown = true;
            }
            HPX_TEST(exception_thrown);
        }

        // let the user polling go out of scope
    }

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    auto result = hpx::init(argc, argv);

    MPI_Finalize();

    return result || hpx::util::report_errors();
}
