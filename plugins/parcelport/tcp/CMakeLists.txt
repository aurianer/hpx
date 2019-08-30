# Copyright (c) 2007-2013 Hartmut Kaiser
# Copyright (c) 2014-2015 Thomas Heller
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

include(HPX_AddLibrary)

if(HPX_WITH_PARCELPORT_TCP)
  if (TARGET hpx::apex)
    set(apex_dep hpx::apex)
  endif()
  hpx_debug("add_parcelport_tcp_module")
  include(HPX_AddParcelport)
  add_parcelport(tcp
    STATIC
    SOURCES
      "${PROJECT_SOURCE_DIR}/plugins/parcelport/tcp/connection_handler_tcp.cpp"
      "${PROJECT_SOURCE_DIR}/plugins/parcelport/tcp/parcelport_tcp.cpp"
    HEADERS
      "${PROJECT_SOURCE_DIR}/hpx/plugins/parcelport/tcp/connection_handler.hpp"
      "${PROJECT_SOURCE_DIR}/hpx/plugins/parcelport/tcp/locality.hpp"
      "${PROJECT_SOURCE_DIR}/hpx/plugins/parcelport/tcp/receiver.hpp"
      "${PROJECT_SOURCE_DIR}/hpx/plugins/parcelport/tcp/sender.hpp"
    DEPENDENCIES
      hpx_config
      hpx_allocator_support
      hpx_assertion
      hpx_cache
      hpx_concurrency
      hpx_errors
      hpx_parallel_executors
      hpx_timing
      hpx_topology
      ${apex_dep}
    INCLUDE_DIRS
      "${PROJECT_SOURCE_DIR}"
    FOLDER
      "Core/Plugins/Parcelport/Tcp"
    )
endif()
