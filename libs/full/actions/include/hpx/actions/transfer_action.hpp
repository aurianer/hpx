//  Copyright (c) 2007-2020 Hartmut Kaiser
//  Copyright (c)      2011 Bryce Lelbach
//  Copyright (c)      2011 Thomas Heller
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file transfer_action.hpp

#pragma once

#include <hpx/config.hpp>

#if defined(HPX_HAVE_NETWORKING) && !defined(HPX_COMPUTE_DEVICE_CODE)
#include <hpx/actions/apply_helper.hpp>
#include <hpx/actions/transfer_base_action.hpp>
#include <hpx/actions_base/actions_base_support.hpp>
#include <hpx/serialization/input_archive.hpp>
#include <hpx/serialization/output_archive.hpp>
#include <hpx/serialization/serialization_fwd.hpp>
#include <hpx/serialization/traits/needs_automatic_registration.hpp>
#include <hpx/threading_base/thread_helpers.hpp>
#include <hpx/threading_base/thread_init_data.hpp>
#include <hpx/type_support/pack.hpp>

#include <cstddef>
#include <cstdint>
#include <utility>

#include <hpx/config/warnings_prefix.hpp>

namespace hpx { namespace actions {

    /// \cond NOINTERNAL

    ///////////////////////////////////////////////////////////////////////////
    template <typename Action>
    struct transfer_action : transfer_base_action<Action>
    {
    public:
        HPX_NON_COPYABLE(transfer_action);

        typedef transfer_base_action<Action> base_type;

    public:
        // construct an empty transfer_action to avoid serialization overhead
        transfer_action() = default;

        // construct an action from its arguments
        template <typename... Ts>
        explicit transfer_action(Ts&&... vs);

        template <typename... Ts>
        transfer_action(threads::thread_priority priority, Ts&&... vs);

        bool has_continuation() const override;

        /// The \a get_thread_function constructs a proper thread function for
        /// a \a thread, encapsulating the functionality and the arguments
        /// of the action it is called for.
        ///
        /// \param lva    [in] This is the local virtual address of the
        ///               component the action has to be invoked on.
        ///
        /// \returns      This function returns a proper thread function usable
        ///               for a \a thread.
        ///
        /// \note This \a get_thread_function will be invoked to retrieve the
        ///       thread function for an action which has to be invoked without
        ///       continuations.
        template <std::size_t... Is>
        threads::thread_function_type get_thread_function(
            util::index_pack<Is...>, naming::id_type&& target,
            naming::address::address_type lva,
            naming::address::component_type comptype);

        threads::thread_function_type get_thread_function(
            naming::id_type&& target, naming::address::address_type lva,
            naming::address::component_type comptype) override;

        template <std::size_t... Is>
        void schedule_thread(util::index_pack<Is...>,
            naming::gid_type const& target_gid,
            naming::address::address_type lva,
            naming::address::component_type comptype, std::size_t num_thread);

        // schedule a new thread
        void schedule_thread(naming::gid_type const& target_gid,
            naming::address::address_type lva,
            naming::address::component_type comptype,
            std::size_t num_thread) override;

        // serialization support
        // loading ...
        void load(hpx::serialization::input_archive& ar) override;

        // saving ...
        void save(hpx::serialization::output_archive& ar) override;

        void load_schedule(serialization::input_archive& ar,
            naming::gid_type&& target, naming::address_type lva,
            naming::component_type comptype, std::size_t num_thread,
            bool& deferred_schedule) override;
    };
    /// \endcond

    template <typename Action>
    template <typename... Ts>
    transfer_action<Action>::transfer_action(Ts&&... vs)
      : base_type(std::forward<Ts>(vs)...)
    {
    }

    template <typename Action>
    template <typename... Ts>
    transfer_action<Action>::transfer_action(
        threads::thread_priority priority, Ts&&... vs)
      : base_type(priority, std::forward<Ts>(vs)...)
    {
    }

    template <typename Action>
    bool transfer_action<Action>::has_continuation() const
    {
        return false;
    }

    template <typename Action>
    template <std::size_t... Is>
    threads::thread_function_type transfer_action<Action>::get_thread_function(
        util::index_pack<Is...>, naming::id_type&& target,
        naming::address::address_type lva,
        naming::address::component_type comptype)
    {
        return base_type::derived_type::construct_thread_function(
            std::move(target), lva, comptype,
            hpx::get<Is>(std::move(this->arguments_))...);
    }

    template <typename Action>
    threads::thread_function_type transfer_action<Action>::get_thread_function(
        naming::id_type&& target, naming::address::address_type lva,
        naming::address::component_type comptype)
    {
        return get_thread_function(
            typename util::make_index_pack<Action::arity>::type(),
            std::move(target), lva, comptype);
    }

    template <typename Action>
    template <std::size_t... Is>
    void transfer_action<Action>::schedule_thread(util::index_pack<Is...>,
        naming::gid_type const& target_gid, naming::address::address_type lva,
        naming::address::component_type comptype, std::size_t /*num_thread*/)
    {
        naming::id_type target;
        if (naming::detail::has_credits(target_gid))
        {
            target = naming::id_type(target_gid, naming::id_type::managed);
        }

        threads::thread_init_data data;
#if defined(HPX_HAVE_THREAD_DESCRIPTION)
        data.description = actions::detail::get_action_name<Action>();
#endif
#if defined(HPX_HAVE_THREAD_PARENT_REFERENCE)
        data.parent_id = this->parent_id_;
        data.parent_locality_id = this->parent_locality_;
#endif
#if defined(HPX_HAVE_APEX)
        data.timer_data = hpx::util::external_timer::new_task(
            data.description, data.parent_locality_id, data.parent_id);
#endif
        applier::detail::apply_helper<typename base_type::derived_type>::call(
            std::move(data), target, lva, comptype, this->priority_,
            std::move(hpx::get<Is>(this->arguments_))...);
    }

    template <typename Action>
    void transfer_action<Action>::schedule_thread(
        naming::gid_type const& target_gid, naming::address::address_type lva,
        naming::address::component_type comptype, std::size_t num_thread)
    {
        schedule_thread(typename util::make_index_pack<Action::arity>::type(),
            target_gid, lva, comptype, num_thread);

        // keep track of number of invocations
        this->increment_invocation_count();
    }

    template <typename Action>
    void transfer_action<Action>::load(hpx::serialization::input_archive& ar)
    {
        this->load_base(ar);
    }

    template <typename Action>
    void transfer_action<Action>::save(hpx::serialization::output_archive& ar)
    {
        this->save_base(ar);
    }

    template <typename Action>
    void transfer_action<Action>::load_schedule(
        serialization::input_archive& ar, naming::gid_type&& target,
        naming::address_type lva, naming::component_type comptype,
        std::size_t num_thread, bool& deferred_schedule)
    {
        // First, serialize, then schedule
        load(ar);

        if (deferred_schedule)
        {
            // If this is a direct action and deferred schedule was requested,
            // that is we are not the last parcel, return immediately
            if (base_type::direct_execution::value)
            {
                return;
            }
            else
            {
                // If this is not a direct action, we can safely set
                // deferred_schedule to false
                deferred_schedule = false;
            }
        }

        schedule_thread(std::move(target), lva, comptype, num_thread);
    }
}}    // namespace hpx::actions

namespace hpx { namespace traits {
    /// \cond NOINTERNAL
    template <typename Action>
    struct needs_automatic_registration<hpx::actions::transfer_action<Action>>
      : needs_automatic_registration<Action>
    {
    };
    /// \endcond
}}    // namespace hpx::traits

#include <hpx/config/warnings_suffix.hpp>

#endif
