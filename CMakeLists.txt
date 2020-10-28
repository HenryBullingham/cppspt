# Copyright(C) 2020 Henry Bullingham
# This file is subject to the license terms in the LICENSE file
# found in the top - level directory of this distribution.


cmake_minimum_required(VERSION 3.1)

project(cppspt)

set(header_files 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cppspt/cppspt.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cppspt/cppspt_category.hpp
)

add_library(cppspt INTERFACE)
target_sources(cppspt INTERFACE "$<BUILD_INTERFACE:${header_files}>")
target_include_directories(cppspt INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>")
target_include_directories(cppspt SYSTEM INTERFACE "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include/>")


# Setup package config
include( CMakePackageConfigHelpers )
set(CONFIG_PACKAGE_INSTALL_DIR lib/cmake/cppspt)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/cppspt-config-version.cmake
    VERSION 0.1.0
    COMPATIBILITY SameMajorVersion)

# Install target
install(DIRECTORY include/cppspt DESTINATION include)

enable_testing()
install(TARGETS cppspt
    EXPORT cppspt-targets
    DESTINATION lib)

install( EXPORT cppspt-targets
    DESTINATION
    ${CONFIG_PACKAGE_INSTALL_DIR})

install( FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/cppspt-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/cppspt-config-version.cmake
    DESTINATION
    ${CONFIG_PACKAGE_INSTALL_DIR} )
    
option(cppspt_BUILD_TEST "build tests" ON)
if(${cppspt_BUILD_TEST})
    add_subdirectory(test/)
endif()

option(cppspt_BUILD_EXAMPLES "build examples" ON)
if(${cppspt_BUILD_EXAMPLES})
    add_subdirectory(examples/)
endif()
