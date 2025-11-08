// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "beman/monadics/detail/or_else.hpp"

#include "helpers/myexpected.hpp"

#include "catch2/catch_test_macros.hpp"

namespace beman::monadics::tests {

TEST_CASE("has-box-traits") { STATIC_REQUIRE(has_box_traits<helpers::expected<void, int>>); }

TEST_CASE("void-value-return-same-error") {
    constexpr auto result =
        helpers::expected<void, int>{} | or_else([](auto e) { return helpers::expected<void, int>(e * 2); });

    STATIC_REQUIRE(std::same_as<decltype(result), const helpers::expected<void, int>>);
    STATIC_REQUIRE(result.has_value());
}

TEST_CASE("void-value-return-another-error") {
    constexpr auto result =
        helpers::expected<void, int>{} | or_else([](auto e) { return helpers::expected<void, double>(e * 2.0); });

    STATIC_REQUIRE(std::same_as<decltype(result), const helpers::expected<void, double>>);
    STATIC_REQUIRE(result.has_value() == true);
}

TEST_CASE("void-value-return-same-error-2") {
    constexpr auto result =
        helpers::expected<void, int>{20} | or_else([](auto e) { return helpers::expected<void, int>(e * 2); });

    STATIC_REQUIRE(std::same_as<decltype(result), const helpers::expected<void, int>>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 40);
}

TEST_CASE("void-value-return-another-error-2") {
    constexpr auto result =
        helpers::expected<void, int>{1} | or_else([](auto e) { return helpers::expected<void, double>(e * 2.0); });

    STATIC_REQUIRE(std::same_as<decltype(result), const helpers::expected<void, double>>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 2.0);
}

TEST_CASE("value-return-same-error") {
    constexpr auto result =
        helpers::expected<int, double>{} | or_else([](auto e) { return helpers::expected<int, double>(e + 1); });

    STATIC_REQUIRE(std::same_as<decltype(result), const helpers::expected<int, double>>);
    STATIC_REQUIRE(result.has_value());
}

TEST_CASE("value-return-another-error") {
    constexpr auto result =
        helpers::expected<int, double>{} | or_else([](auto e) { return helpers::expected<int, float>(float(e)); });

    STATIC_REQUIRE(std::same_as<decltype(result), const helpers::expected<int, float>>);
    STATIC_REQUIRE(result.has_value() == true);
}

TEST_CASE("value-return-same-error-2") {
    constexpr auto result =
        helpers::expected<int, double>{2.0} | or_else([](auto e) { return helpers::expected<int, double>(e + 1); });

    STATIC_REQUIRE(std::same_as<decltype(result), const helpers::expected<int, double>>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 3.0);
}

TEST_CASE("value-return-another-error-2") {
    constexpr auto result = helpers::expected<double, char>{'c'} |
                            or_else([](auto e) { return helpers::expected<double, int>(static_cast<int>(e)); });

    STATIC_REQUIRE(std::same_as<decltype(result), const helpers::expected<double, int>>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == static_cast<char>('c'));
}

} // namespace beman::monadics::tests
