# Copyright (c) 2018 Christopher Hinz
# Copyright (c) 2014 Thomas Heller
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

include(CMakePackageConfigHelpers)
include(HPX_GeneratePackageUtils)

set(CMAKE_DIR "cmake-${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}"
  CACHE STRING "directory (in share), where to put FindHPX cmake module")

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/lib/cmake/${HPX_PACKAGE_NAME}/${HPX_PACKAGE_NAME}ConfigVersion.cmake"
  VERSION ${HPX_VERSION}
  COMPATIBILITY AnyNewerVersion
)

# Export HPXTargets in the build directory
export(TARGETS ${HPX_EXPORT_TARGETS}
  FILE "${CMAKE_CURRENT_BINARY_DIR}/lib/cmake/${HPX_PACKAGE_NAME}/HPXTargets.cmake"
)

# Export HPXTargets in the install directory
install(
  EXPORT HPXTargets
  FILE HPXTargets.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${HPX_PACKAGE_NAME}
)

# Generate the pkconfig files for HPX_APPLICATION (both for build and install)
hpx_generate_pkgconfig_from_target(hpx::application hpx_application TRUE)
hpx_generate_pkgconfig_from_target(hpx::application hpx_application FALSE)
# Generate the pkconfig files for HPX_COMPONENT (both for build and install)
hpx_generate_pkgconfig_from_target(hpx::component hpx_component TRUE)
hpx_generate_pkgconfig_from_target(hpx::component hpx_component FALSE)

# Install dir
configure_file(cmake/templates/${HPX_PACKAGE_NAME}Config.cmake.in
  "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${HPX_PACKAGE_NAME}Config.cmake"
  ESCAPE_QUOTES @ONLY)
configure_file(cmake/templates/hpxcxx.in
  "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/hpxcxx"
  @ONLY)
# Build dir
configure_file(cmake/templates/${HPX_PACKAGE_NAME}Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/lib/cmake/${HPX_PACKAGE_NAME}/${HPX_PACKAGE_NAME}Config.cmake"
  ESCAPE_QUOTES @ONLY)
configure_file(cmake/templates/hpxcxx.in
  "${CMAKE_CURRENT_BINARY_DIR}/bin/hpxcxx"
  @ONLY)

# Configure macros for the install dir ...
set(HPX_CMAKE_MODULE_PATH "${CMAKE_INSTALL_FULL_LIBDIR}/cmake/${HPX_PACKAGE_NAME}")
configure_file(cmake/templates/HPXMacros.cmake.in
  "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/HPXMacros.cmake"
  ESCAPE_QUOTES @ONLY)
# ... and the build dir
set(HPX_CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
configure_file(cmake/templates/HPXMacros.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/lib/cmake/${HPX_PACKAGE_NAME}/HPXMacros.cmake"
  ESCAPE_QUOTES @ONLY)

install(
  FILES
    "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${HPX_PACKAGE_NAME}Config.cmake"
    "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/HPXMacros.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/lib/cmake/${HPX_PACKAGE_NAME}/${HPX_PACKAGE_NAME}ConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${HPX_PACKAGE_NAME}
  COMPONENT cmake
)

string(TOLOWER ${CMAKE_BUILD_TYPE} build_type)
install(
  FILES
    ${OUTPUT_DIR_PC}/hpx_application_${build_type}.pc
    ${OUTPUT_DIR_PC}/hpx_component_${build_type}.pc
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
  COMPONENT pkgconfig
)

# Temporary (to deprecate gradually)
install(
  FILES
    ${OUTPUT_DIR_PC}/hpx_application.pc
    ${OUTPUT_DIR_PC}/hpx_component.pc
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
  CONFIGURATIONS Release
  COMPONENT pkgconfig
)

install(
  FILES
    "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/hpxcxx"
  DESTINATION ${CMAKE_INSTALL_BINDIR}
  COMPONENT compiler_wrapper
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE
)

install(
  FILES
    "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/hpx_bazel_defs.bzl"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/bazel
  COMPONENT bazel
)
