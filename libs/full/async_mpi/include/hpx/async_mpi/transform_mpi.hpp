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
#include <hpx/concepts/concepts.hpp>
#include <hpx/execution_base/receiver.hpp>
#include <hpx/execution_base/sender.hpp>
#include <hpx/functional/tag_fallback_dispatch.hpp>
#include <hpx/functional/invoke.hpp>
#include <hpx/functional/traits/is_invocable.hpp>

#include <mpi.h>

#include <exception>

namespace hpx { namespace mpi { namespace experimental {
    namespace detail {

        template <typename R, typename F>
        struct transform_mpi_receiver
        {
            std::decay_t<R> r;
            std::decay_t<F> f;

            template <typename R_, typename F_>
            transform_mpi_receiver(R_&& r, F_&& f)
              : r(std::forward<R_>(r)), f(std::forward<F_>(f))
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

            template <typename... Ts,
                     typename = std::enable_if_t<hpx::is_invocable_v<F, Ts..., MPI_Request*>>>
            void set_value(Ts&&... ts) && noexcept
            {
                hpx::detail::try_catch_exception_ptr(
                    [&]() {
                        MPI_Request request;
                        HPX_INVOKE(f, std::forward<Ts>(ts)..., &request);
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

        template <typename S, typename F>
        struct transform_mpi_sender
        {
            std::decay_t<S> s;
            std::decay_t<F> f;

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
                    transform_mpi_receiver<R, F>(std::forward<R>(r), std::move(f)));
            }
        };
    }    // namespace detail

    HPX_INLINE_CONSTEXPR_VARIABLE struct transform_mpi_t final
      : hpx::functional::tag_fallback<transform_mpi_t>
    {
    private:
        template <typename S, typename F,
                HPX_CONCEPT_REQUIRES_(
                    hpx::execution::experimental::is_sender_v<S>
                )>
        friend constexpr HPX_FORCEINLINE auto tag_fallback_dispatch(
            transform_mpi_t, S&& s, F&& f)
        {
            return detail::transform_mpi_sender<S, F>{std::forward<S>(s), std::forward<F>(f)};
        }
    } transform_mpi{};
}}}    // namespace hpx::mpi::experimental
