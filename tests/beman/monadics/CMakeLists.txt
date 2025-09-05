# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

find_package(Catch2 REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(Catch)

add_executable(beman.monadics.tests.monadics)
target_sources(beman.monadics.tests.monadics PRIVATE monadics.test.cpp)
target_link_libraries(
    beman.monadics.tests.monadics
    PRIVATE beman::monadics Catch2::Catch2WithMain
)

catch_discover_tests(beman.monadics.tests.monadics
  TEST_PREFIX "beman.monadics.tests.monadics."
)
