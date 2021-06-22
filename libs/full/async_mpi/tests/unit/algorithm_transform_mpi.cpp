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
            // MPI function pointer
            int data = 0, count = 1;
            if (rank == 0)
            {
                data = 42;
            }
            auto s = mpi::transform_mpi(ex::just(&data, count, datatype, 0, comm), MPI_Ibcast);
            auto result = ex::sync_wait(s);
            if (rank != 0)
            {
                HPX_TEST_EQ(data, 42);
            }
            if (rank == 0)
            {
                HPX_TEST(result == MPI_SUCCESS);
            }
        }

        {
            // Lambda
            int data = 0, count = 1;
            if (rank == 0)
            {
                data = 42;
            }
            auto s = mpi::transform_mpi(ex::just(&data, count, datatype, 0, comm),
                    [](int *data, int count, MPI_Datatype datatype, int i,
                        MPI_Comm comm, MPI_Request* request)
                    {
                        return MPI_Ibcast(data, count, datatype, i, comm, request);
                    });
            auto result = ex::sync_wait(s);
            if (rank != 0)
            {
                HPX_TEST_EQ(data, 42);
            }
            if (rank == 0)
            {
                HPX_TEST(result == MPI_SUCCESS);
            }
        }

        // TODO: add a test that verifies the mpi int result when failure

        // TODO: add a test that uses .on before or after the mpi sender

        // TODO: add an overload to verify the tag_dispatch overload

        // TODO: add a pipe overload test

        // Failure path
        {
            bool exception_thrown = false;
            try
            {
                mpi::transform_mpi(error_sender{}, MPI_Ibcast) | ex::sync_wait();
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
