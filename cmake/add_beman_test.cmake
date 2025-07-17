# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

include_guard(GLOBAL)

find_package(GTest REQUIRED)
include(GoogleTest)

function(add_beman_test name)
  set(target "beman.monadics.tests.${name}")

  add_executable(${target})
  target_sources(${target}
    PRIVATE
      ${name}.test.cpp
  )

  target_link_libraries(${target}
    PRIVATE
      beman::monadics
      GTest::gtest
      GTest::gtest_main
  )

  gtest_discover_tests(${target})
endfunction()
