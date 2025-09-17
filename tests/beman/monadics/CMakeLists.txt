# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

find_package(Catch2 REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(Catch)

set(tests monadics box_traits)

foreach(test ${tests})
    add_executable(beman.monadics.tests.${test})
    target_sources(beman.monadics.tests.${test} PRIVATE ${test}.test.cpp)
    target_link_libraries(
        beman.monadics.tests.${test}
        PRIVATE beman::monadics Catch2::Catch2WithMain
    )

    target_compile_options(
        beman.monadics.tests.${test}
        PRIVATE "$<$<CXX_COMPILER_ID:GNU>:-fconcepts-diagnostics-depth=10>"
    )

    catch_discover_tests(beman.monadics.tests.${test}
      TEST_PREFIX "beman.monadics.tests.${test}."
    )
endforeach()
