# Copyright (c) 2019 Ste||ar Group
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

function(parse_libraries libraries name)
  # Parse MSVC libraries to avoid cmake bug
  if (MSVC)
    string(REGEX REPLACE "optimized" ";optimized-" _tmp_libraries "${libraries}")
    string(REGEX REPLACE "debug" ";debug-" _tmp_libraries "${_tmp_libraries}")
    string(REGEX REPLACE "general" ";general-" _tmp_libraries "${_tmp_libraries}")
    if ("CMAKE_BUILD_TYPE" MATCHES "Rel")
      list(FILTER _tmp_libraries INCLUDE REGEX optimized-|general-)
    elseif ("CMAKE_BUILD_TYPE" MATCHES "Debug")
      list(FILTER _tmp_libraries INCLUDE REGEX debug-|general-)
    else()
      # Should not be processed
      list(FILTER _tmp_libraries INCLUDE REGEX general-)
    endif()
    string(REGEX REPLACE "optimized-" "" _tmp_libraries "${_tmp_libraries}")
    string(REGEX REPLACE "debug-" "" _tmp_libraries "${_tmp_libraries}")
    string(REGEX REPLACE "general-" "" _tmp_libraries "${_tmp_libraries}")
    set(${name} "${_tmp_libraries}" PARENT_SCOPE)
  else()
    set(${name} "${libraries}" PARENT_SCOPE)
  endif(MSVC)
endfunction(parse_libraries)

# In order to fix the cmake bug, in case interface_link_libraries, we filter the
# libraries and then we call the old set_property with the filtered libraries
function(set_property)
  # Parse arguments
  set(options APPEND PROPERTY)
  set(one_value_args TARGET)
  set(multi_value_args INTERFACE_LINK_LIBRARIES)
  cmake_parse_arguments(my_props "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})
  if (NOT my_props_INTERFACE_LINK_LIBRARIES)
    # No bug, we directly call the old function
    _set_property(${ARGN})
  else()
    parse_libraries("${my_props_INTERFACE_LINK_LIBRARIES}" filtered_libraries)
    if (my_props_APPEND)
      set(_append APPEND)
    endif()
    _set_property(TARGET ${my_props_TARGET} ${_append} PROPERTY INTERFACE_LINK_LIBRARIES ${filtered_libraries})
  endif()
endfunction(set_property)
