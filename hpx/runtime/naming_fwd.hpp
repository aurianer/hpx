//  Copyright (c) 2007-2016 Hartmut Kaiser
//  Copyright (c) 2011      Bryce Lelbach
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_RUNTIME_NAMING_FWD_HPP
#define HPX_RUNTIME_NAMING_FWD_HPP

#include <hpx/config.hpp>
#include <hpx/runtime/agas_fwd.hpp>
#include <hpx/util/naming_types_fwd.hpp>

#include <cstdint>

namespace hpx
{
    /// \namespace naming
    ///
    /// The namespace \a naming contains all definitions needed for the AGAS
    /// (Active Global Address Space) service.
    namespace naming
    {
        using resolver_client = agas::addressing_service;

        struct HPX_EXPORT gid_type;
        struct HPX_EXPORT id_type;
        struct HPX_EXPORT address;

        HPX_API_EXPORT resolver_client& get_agas_client();

        // tag used to mark serialization archive during check-pointing
        struct checkpointing_tag {};
    }

    ///////////////////////////////////////////////////////////////////////////
    // Pulling important types into the main namespace
    using naming::id_type;
}

#endif
