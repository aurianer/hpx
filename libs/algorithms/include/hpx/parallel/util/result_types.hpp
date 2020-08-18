//  Copyright (c) 2020 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file parallel/container_algorithms/copy.hpp

#pragma once

#include <hpx/config.hpp>
#include <hpx/modules/datastructures.hpp>
#include <hpx/modules/futures.hpp>

#include <type_traits>
#include <utility>

namespace hpx { namespace parallel { namespace util {

    ///////////////////////////////////////////////////////////////////////////
    template <typename I1, typename I2>
    struct in_in_result
    {
        HPX_NO_UNIQUE_ADDRESS I1 in1;
        HPX_NO_UNIQUE_ADDRESS I2 in2;

        template <typename II1, typename II2,
            typename Enable = typename std::enable_if<
                std::is_convertible<I1 const&, II1&>::value &&
                std::is_convertible<I2 const&, II2&>::value>::type>
        constexpr operator in_in_result<II1, II2>() const&
        {
            return {in1, in2};
        }

        template <typename II1, typename II2,
            typename Enable =
                typename std::enable_if<std::is_convertible<I1, II1>::value &&
                    std::is_convertible<I2, II2>::value>::type>
        constexpr operator in_in_result<II1, II2>() &&
        {
            return {std::move(in1), std::move(in2)};
        }

        template <typename Archive>
        void serialize(Archive& ar, unsigned)
        {
            // clang-format off
            ar & in1 & in2;
            // clang-format on
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename I, typename O>
    struct in_out_result
    {
        HPX_NO_UNIQUE_ADDRESS I in;
        HPX_NO_UNIQUE_ADDRESS O out;

        template <typename I2, typename O2,
            typename Enable = typename std::enable_if<
                std::is_convertible<I const&, I2&>::value &&
                std::is_convertible<O const&, O2&>::value>::type>
        constexpr operator in_out_result<I2, O2>() const&
        {
            return {in, out};
        }

        template <typename I2, typename O2,
            typename Enable =
                typename std::enable_if<std::is_convertible<I, I2>::value &&
                    std::is_convertible<O, O2>::value>::type>
        constexpr operator in_out_result<I2, O2>() &&
        {
            return {std::move(in), std::move(out)};
        }

        template <typename Archive>
        void serialize(Archive& ar, unsigned)
        {
            // clang-format off
            ar & in & out;
            // clang-format on
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename I1, typename I2, typename O>
    struct in_in_out_result
    {
        HPX_NO_UNIQUE_ADDRESS I1 in1;
        HPX_NO_UNIQUE_ADDRESS I2 in2;
        HPX_NO_UNIQUE_ADDRESS O out;

        template <typename II1, typename II2, typename OO,
            typename Enable = typename std::enable_if<
                std::is_convertible<I1 const&, II1&>::value &&
                std::is_convertible<I2 const&, II2&>::value &&
                std::is_convertible<O const&, OO&>::value>::type>
        constexpr operator in_in_out_result<II1, II2, OO>() const&
        {
            return {in1, in2, out};
        }

        template <typename II1, typename II2, typename OO,
            typename Enable =
                typename std::enable_if<std::is_convertible<I1, II1>::value &&
                    std::is_convertible<I2, II2>::value &&
                    std::is_convertible<O, OO>::value>::type>
        constexpr operator in_in_out_result<II1, II2, OO>() &&
        {
            return {std::move(in1), std::move(in2), std::move(out)};
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename I, typename F>
    struct in_fun_result
    {
        HPX_NO_UNIQUE_ADDRESS I in;
        HPX_NO_UNIQUE_ADDRESS F fun;

        template <typename I2, typename F2,
            typename Enable = typename std::enable_if<
                std::is_convertible<I const&, I2&>::value &&
                std::is_convertible<F const&, F2&>::value>::type>
        constexpr operator in_fun_result<I2, F2>() const&
        {
            return {in, fun};
        }

        template <typename I2, typename F2,
            typename Enable =
                typename std::enable_if<std::is_convertible<I, I2>::value &&
                    std::is_convertible<F, F2>::value>::type>
        constexpr operator in_fun_result<I2, F2>() &&
        {
            return {std::move(in), std::move(fun)};
        }

        template <typename Archive>
        void serialize(Archive& ar, unsigned)
        {
            // clang-format off
            ar & in & fun;
            // clang-format on
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    namespace detail {
        template <typename ZipIter>
        in_out_result<typename hpx::util::tuple_element<0,
                          typename ZipIter::iterator_tuple_type>::type,
            typename hpx::util::tuple_element<1,
                typename ZipIter::iterator_tuple_type>::type>
        get_in_out_result(ZipIter&& zipiter)
        {
            using iterator_tuple_type = typename ZipIter::iterator_tuple_type;

            using result_type = in_out_result<
                typename hpx::util::tuple_element<0, iterator_tuple_type>::type,
                typename hpx::util::tuple_element<1,
                    iterator_tuple_type>::type>;

            iterator_tuple_type t = zipiter.get_iterator_tuple();
            return result_type{hpx::util::get<0>(t), hpx::util::get<1>(t)};
        }

        template <typename ZipIter>
        hpx::future<
            in_out_result<typename hpx::util::tuple_element<0,
                              typename ZipIter::iterator_tuple_type>::type,
                typename hpx::util::tuple_element<1,
                    typename ZipIter::iterator_tuple_type>::type>>
        get_in_out_result(hpx::future<ZipIter>&& zipiter)
        {
            using iterator_tuple_type = typename ZipIter::iterator_tuple_type;

            using result_type = in_out_result<
                typename hpx::util::tuple_element<0, iterator_tuple_type>::type,
                typename hpx::util::tuple_element<1,
                    iterator_tuple_type>::type>;

            return lcos::make_future<result_type>(
                std::move(zipiter), [](ZipIter zipiter) {
                    return get_in_out_result(std::move(zipiter));
                });
        }
    }    // namespace detail

    template <typename ZipIter>
    in_in_out_result<typename hpx::util::tuple_element<0,
                         typename ZipIter::iterator_tuple_type>::type,
        typename hpx::util::tuple_element<1,
            typename ZipIter::iterator_tuple_type>::type,
        typename hpx::util::tuple_element<2,
            typename ZipIter::iterator_tuple_type>::type>
    get_in_in_out_result(ZipIter&& zipiter)
    {
        using iterator_tuple_type = typename ZipIter::iterator_tuple_type;

        using result_type = in_in_out_result<
            typename hpx::util::tuple_element<0, iterator_tuple_type>::type,
            typename hpx::util::tuple_element<1, iterator_tuple_type>::type,
            typename hpx::util::tuple_element<2, iterator_tuple_type>::type>;

        iterator_tuple_type t = zipiter.get_iterator_tuple();
        return result_type{
            hpx::util::get<0>(t), hpx::util::get<1>(t), hpx::util::get<2>(t)};
    }

    template <typename ZipIter>
    hpx::future<
        in_in_out_result<typename hpx::util::tuple_element<0,
                             typename ZipIter::iterator_tuple_type>::type,
            typename hpx::util::tuple_element<1,
                typename ZipIter::iterator_tuple_type>::type,
            typename hpx::util::tuple_element<2,
                typename ZipIter::iterator_tuple_type>::type>>
    get_in_in_out_result(hpx::future<ZipIter>&& zipiter)
    {
        using iterator_tuple_type = typename ZipIter::iterator_tuple_type;

        using result_type = in_in_out_result<
            typename hpx::util::tuple_element<0, iterator_tuple_type>::type,
            typename hpx::util::tuple_element<1, iterator_tuple_type>::type,
            typename hpx::util::tuple_element<2, iterator_tuple_type>::type>;

        return lcos::make_future<result_type>(
            std::move(zipiter), [](ZipIter zipiter) {
                return get_in_in_out_result(std::move(zipiter));
            });
    }

    ///////////////////////////////////////////////////////////////////////
    template <typename I, typename O>
    O get_second_element(util::in_out_result<I, O>&& p)
    {
        return p.out;
    }

    template <typename I, typename O>
    hpx::future<O> get_second_element(
        hpx::future<util::in_out_result<I, O>>&& f)
    {
        return lcos::make_future<O>(
            std::move(f), [](util::in_out_result<I, O>&& p) { return p.out; });
    }

}}}      // namespace hpx::parallel::util

namespace hpx { namespace ranges {
    using hpx::parallel::util::in_fun_result;
    using hpx::parallel::util::in_out_result;
}}    // namespace hpx::ranges
