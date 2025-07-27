# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

add_beman_library(monadics)

target_sources(
    ${beman_lib_target}
    PRIVATE concepts/decomposable.cpp concepts/instance_of.cpp
)

target_sources(
    ${beman_lib_target}
    PUBLIC
        FILE_SET HEADERS
        FILES
            ../../../include/beman/monadics/concepts/decomposable.hpp
            ../../../include/beman/monadics/concepts/instance_of.hpp
            ../../../include/beman/monadics/concepts/same_template_as.hpp
            ../../../include/beman/monadics/type_traits/decompose.hpp
            ../../../include/beman/monadics/type_traits/rebind_value.hpp
)
