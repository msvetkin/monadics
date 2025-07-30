// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "beman/monadics/transform.hpp"

#include <catch2/catch_test_macros.hpp>

#include "std.hpp"

namespace beman::monadics::tests {

TEST_CASE("opt-value") {
    using T      = std::optional<int>;
    using traits = box_traits_for<T>;
    constexpr T box{10};

    constexpr auto box2 = box | transform([](auto v) { return v * 3; });

    STATIC_REQUIRE(traits::has_value(box2));
    STATIC_REQUIRE(traits::value(box2) == 30);
    STATIC_REQUIRE(traits::support_error == false);
}

} // namespace beman::monadics::tests
