// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include "beman/monadics/detail/transform.hpp"

#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("hm") {
    // constexpr auto result = [] () {
    // int value{10};
    // const auto result = &value | transform([] (auto &&v) {
    // static int v2 = 555;
    // v2 = v2 + v;
    // return &v2;
    // });

    // return *result;
    // };

    // STATIC_REQUIRE(result == 565);
}

} // namespace beman::monadics::tests
