// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include "beman/monadics/detail/transform.hpp"

#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("with-value") {
    constexpr auto result = std::optional{10} | transform([](auto&& value) { return value * 2.0; });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::optional<double>>);
    STATIC_REQUIRE(result.has_value());
    STATIC_REQUIRE(result.value() == 20.0);
}

TEST_CASE("with-nullopt") {
    constexpr auto result = std::optional<int>{} | transform([](auto&& value) { return (value - 1) * 0.5; });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::optional<double>>);
    STATIC_REQUIRE(result.has_value() == false);
}

} // namespace beman::monadics::tests
