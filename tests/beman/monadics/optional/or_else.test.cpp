// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include "beman/monadics/detail/or_else.hpp"

#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("with-value") {
    constexpr auto result = std::optional{10} | or_else([]() { return std::optional{100}; });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::optional<int>>);
    STATIC_REQUIRE(result.has_value());
    STATIC_REQUIRE(result.value() == 10);
}

// should not be allowed
TEST_CASE("with-value-bad-return-type") {
    constexpr auto result = std::optional<int>{} | or_else([]() { return std::optional{'c'}; });
    // STATIC_REQUIRE(std::same_as<decltype(result), const std::optional<int>>);
    STATIC_REQUIRE(result.has_value());
    // STATIC_REQUIRE(result.value() == 100);
}

TEST_CASE("without-value") {
    constexpr auto result = std::optional<int>{} | or_else([]() { return std::optional{100}; });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::optional<int>>);
    STATIC_REQUIRE(result.has_value());
    STATIC_REQUIRE(result.value() == 100);
}

} // namespace beman::monadics::tests
