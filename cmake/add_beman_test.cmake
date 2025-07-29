# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

include_guard(GLOBAL)

find_package(Catch2 REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(Catch)

function(_get_beman_subdir_target out_subdir_target)
  cmake_path(
    RELATIVE_PATH CMAKE_CURRENT_LIST_DIR
    BASE_DIRECTORY ${PROJECT_SOURCE_DIR}/tests
    OUTPUT_VARIABLE subdir_target
  )

  string(REPLACE "/" "." subdir_target "tst.${subdir_target}")

  if (NOT TARGET ${subdir_target})
    add_custom_target(${subdir_target})
  endif()

  set(${out_subdir_target} ${subdir_target} PARENT_SCOPE)
endfunction()

function(add_beman_test name)
  _get_beman_subdir_target(subdir_target)
  set(test_target "${subdir_target}.${name}")

  add_executable(${test_target})
  add_dependencies(${subdir_target} ${test_target})

  target_sources(${test_target}
    PRIVATE
      ${name}.test.cpp
  )

  target_link_libraries(${test_target}
    PRIVATE
      beman::monadics
      Catch2::Catch2WithMain
  )

  catch_discover_tests(${test_target}
    TEST_PREFIX "${test_target}."
  )
endfunction()
