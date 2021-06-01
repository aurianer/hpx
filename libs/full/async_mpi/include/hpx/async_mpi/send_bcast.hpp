//  Copyright (c) 2007-2021 Hartmut Kaiser
//  Copyright (c) 2021 Giannis Gonidelis
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file parallel/algorithms/transform_xxx.hpp

#pragma once

#include <hpx/config.hpp>
#include <hpx/async_mpi/mpi_future.hpp>
#include <hpx/execution_base/receiver.hpp>
#include <hpx/execution_base/sender.hpp>
#include <hpx/functional/tag_fallback_dispatch.hpp>

#include <mpi.h>

#include <exception>

namespace hpx { namespace mpi { namespace experimental {
    namespace detail {

        template <typename R>
        struct send_bcast_receiver
        {
            std::decay_t<R> r;

            template <typename R_>
            send_bcast_receiver(R_&& r)
              : r(std::forward<R_>(r))
            {
            }

            template <typename E>
            void set_error(E&& e) noexcept
            {
                hpx::execution::experimental::set_error(
                    std::move(r), std::forward<E>(e));
            }

            void set_done() noexcept
            {
                hpx::execution::experimental::set_done(std::move(r));
            };

            template <typename... Ts>
            void set_value(Ts&&... ts) && noexcept
            {
                hpx::detail::try_catch_exception_ptr(
                    [&]() {
                        MPI_Request request;
                        MPI_Ibcast(std::forward<Ts>(ts)..., &request);
                        hpx::mpi::experimental::get_future(request)
                            .then([r = std::move(r)](hpx::future<void>&& dummy) mutable
                            {
                                HPX_UNUSED(dummy);
                                hpx::execution::experimental::set_value(std::move(r));
                            });
                    },
                    [&](std::exception_ptr ep) {
                        hpx::execution::experimental::set_error(
                            std::move(r), std::move(ep));
                    });
            }
        };

        template <typename S>
        struct send_bcast_sender
        {
            std::decay_t<S> s;

            // The sender returned will be void
            template <template <typename...> class Tuple,
                template <typename...> class Variant>
            using value_types = Variant<Tuple<>>;

            template <template <typename...> class Variant>
            using error_types =
                typename hpx::execution::experimental::sender_traits<
                    S>::template error_types<Variant>;

            static constexpr bool sends_done = false;

            template <typename R>
            auto connect(R&& r)
            {
                return hpx::execution::experimental::connect(std::move(s),
                    send_bcast_receiver<R>(std::forward<R>(r)));
            }
        };
    }    // namespace detail

    HPX_INLINE_CONSTEXPR_VARIABLE struct send_bcast_t final
      : hpx::functional::tag_fallback<send_bcast_t>
    {
    private:
        template <typename S>
        friend constexpr HPX_FORCEINLINE auto tag_fallback_dispatch(
            send_bcast_t, S&& s)
        {
            return detail::send_bcast_sender<S>{std::forward<S>(s)};
        }
    } send_bcast{};
}}}    // namespace hpx::mpi::experimental
