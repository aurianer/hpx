//  Copyright (c) 2007-2016 Hartmut Kaiser
//  Copyright (c) 2011      Bryce Lelbach
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_RUNTIME_NAMING_TYPES_FWD_HPP
#define HPX_RUNTIME_NAMING_TYPES_FWD_HPP

#include <hpx/config.hpp>

namespace hpx { namespace naming {

    using component_type = std::int32_t;
    using address_type = std::uint64_t;

    HPX_CONSTEXPR_OR_CONST std::uint32_t invalid_locality_id =
        ~static_cast<std::uint32_t>(0);

}}  // namespace hpx::naming


#endif
