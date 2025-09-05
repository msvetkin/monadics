// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/box_traits.hpp>

#include <catch2/catch_template_test_macros.hpp>

namespace monadics = beman::monadics;

template <>
struct monadics::box_traits<float> {};

TEMPLATE_TEST_CASE_SIG("has-box-traits", "", ((typename T, bool Expected), T, Expected), (int, false), (float, true)) {
    STATIC_REQUIRE(monadics::has_box_traits<T> == Expected);
}
