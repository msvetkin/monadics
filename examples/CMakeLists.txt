# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(ALL_EXAMPLES monadics_std_usage)

message("Examples to be built: ${ALL_EXAMPLES}")

foreach(example ${ALL_EXAMPLES})
    add_executable(beman.monadics.examples.${example})
    target_sources(beman.monadics.examples.${example} PRIVATE ${example}.cpp)
    target_link_libraries(
        beman.monadics.examples.${example}
        PRIVATE beman::monadics
    )
endforeach()
