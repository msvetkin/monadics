# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

add_beman_library(monadics)

target_sources(${beman_lib_target} PRIVATE concepts/decomposable.cpp)

target_sources(
    ${beman_lib_target}
    PUBLIC
        FILE_SET HEADERS
        FILES ../../../include/beman/monadics/concepts/decomposable.hpp
)
