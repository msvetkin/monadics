# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

find_package(Catch2 REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(Catch)

# set(tests monadics box_traits and_then and_then.optional transform or_else transform_error)

# foreach(test ${tests})
    # add_executable(beman.monadics.tests.${test})
    # target_sources(beman.monadics.tests.${test} PRIVATE ${test}.test.cpp)
    # target_link_libraries(
        # beman.monadics.tests.${test}
        # PRIVATE beman::monadics Catch2::Catch2WithMain
    # )

    # target_compile_options(
        # beman.monadics.tests.${test}
        # PRIVATE "$<$<CXX_COMPILER_ID:GNU>:-fconcepts-diagnostics-depth=10>"
    # )

    # catch_discover_tests(beman.monadics.tests.${test}
      # TEST_PREFIX "beman.monadics.tests.${test}."
    # )
# endforeach()

set(tests
  and_then/expected
  and_then/optional
  and_then/shared_ptr
  transform/optional
  transform/shared_ptr
)

foreach(test_path ${tests})
    string(REPLACE "/" ";" test_parts "${test_path}")
    list(GET test_parts 0 test_dir)
    list(GET test_parts 1 test_name)

    set(test_target beman.monadics.tests.${test_dir}.${test_name})

    add_executable(${test_target})
    target_sources(${test_target} PRIVATE ${test_path}.test.cpp)
    target_link_libraries(${test_target}
        PRIVATE beman::monadics Catch2::Catch2WithMain
    )
    target_include_directories(${test_target}
        PRIVATE ${CMAKE_CURRENT_LIST_DIR}
    )
    target_compile_options(
        ${test_target}
        PRIVATE "$<$<CXX_COMPILER_ID:GNU>:-fconcepts-diagnostics-depth=10>"
    )

    catch_discover_tests(${test_target}
      TEST_PREFIX "${test_target}."
    )
endforeach()
