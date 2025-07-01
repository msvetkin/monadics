# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

find_package(GTest REQUIRED)

add_executable(beman.monadics.tests.monadics)
target_sources(beman.monadics.tests.monadics PRIVATE monadics.test.cpp)
target_link_libraries(
    beman.monadics.tests.monadics
    PRIVATE beman::monadics GTest::gtest GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(beman.monadics.tests.monadics)
