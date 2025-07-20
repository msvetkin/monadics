# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

add_beman_library(monadics)

target_sources(${beman_lib_target}
  PRIVATE
    concepts.cpp
    lift.cpp
)

target_sources(${beman_lib_target}
  PUBLIC
    FILE_SET HEADERS
    FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/../../../include/beman/monadics/concepts.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/../../../include/beman/monadics/lift.hpp
)
