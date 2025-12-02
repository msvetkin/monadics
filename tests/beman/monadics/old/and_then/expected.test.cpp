// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define BEMAN_MONADICS_BOX_TRAITS_BUILDER 1
#include "beman/monadics/detail/and_then.hpp"

#include "helpers/myexpected.hpp"

#include "catch2/catch_test_macros.hpp"

namespace beman::monadics::tests {

TEST_CASE("has-box-traits") {
    using Expected = helpers::expected<void, int>;
    using Traits   = box_traits_for<Expected>;
    STATIC_REQUIRE(std::same_as<typename Traits::value_type, void>);
    STATIC_REQUIRE(std::same_as<Traits::error_type, int>);
    STATIC_REQUIRE(std::same_as<Traits::rebind<double>, helpers::expected<double, int>>);
    STATIC_REQUIRE(std::same_as<Traits::rebind_error<double>, helpers::expected<void, double>>);
    STATIC_REQUIRE(std::same_as<decltype(Traits::value(Expected{10})), void>);
    STATIC_REQUIRE(Traits::error(Expected{10}) == 10);
    STATIC_REQUIRE(Traits::lift_error(1) == Expected{1});
    STATIC_REQUIRE(Traits::lift() == Expected{});
}

TEST_CASE("with-void-value-return-non-void-with-value") {
    constexpr auto result =
        helpers::expected<void, int>{} | and_then([]() { return helpers::expected<double, int>(2.0); });

    STATIC_REQUIRE(std::same_as<decltype(result), const helpers::expected<double, int>>);
    STATIC_REQUIRE(result.has_value());
    STATIC_REQUIRE(result.value() == 2.0);
}

TEST_CASE("with-void-value-return-non-void-with-error") {
    constexpr auto result =
        helpers::expected<void, int>{} | and_then([]() { return helpers::expected<double, int>(2); });

    STATIC_REQUIRE(std::same_as<decltype(result), const helpers::expected<double, int>>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 2);
}

TEST_CASE("with-void-value-return-void-with-value") {
    constexpr auto result =
        helpers::expected<void, int>{} | and_then([]() -> helpers::expected<void, int> { return {}; });

    STATIC_REQUIRE(std::same_as<decltype(result), const helpers::expected<void, int>>);
    STATIC_REQUIRE(result.has_value());
}

TEST_CASE("with-void-value-return-void-with-error") {
    constexpr auto result =
        helpers::expected<void, int>{} | and_then([]() -> helpers::expected<void, int> { return {5}; });

    STATIC_REQUIRE(std::same_as<decltype(result), const helpers::expected<void, int>>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 5);
}

TEST_CASE("with-value-return-non-void-with-value") {
    constexpr auto result =
        helpers::expected<int, double>{10}
        | and_then([](auto) -> helpers::expected<std::string_view, double> { return std::string_view{"some"}; });

    STATIC_REQUIRE(std::same_as<decltype(result), const helpers::expected<std::string_view, double>>);
    STATIC_REQUIRE(result.has_value());
    STATIC_REQUIRE(result.value() == "some");
}

TEST_CASE("with-value-return-non-void-with-error") {
    constexpr auto result = helpers::expected<int, double>{10}
                          | and_then([](auto&& v) -> helpers::expected<std::string_view, double> { return v * 2.0; });

    STATIC_REQUIRE(std::same_as<decltype(result), const helpers::expected<std::string_view, double>>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 20.0);
}

TEST_CASE("with-value-return-void-with-value") {
    constexpr auto result =
        helpers::expected<int, double>{10} | and_then([](auto) -> helpers::expected<void, double> { return {}; });

    STATIC_REQUIRE(std::same_as<decltype(result), const helpers::expected<void, double>>);
    STATIC_REQUIRE(result.has_value());
}

TEST_CASE("with-value-return-void-with-error") {
    constexpr auto result =
        helpers::expected<int, double>{10} | and_then([](auto) -> helpers::expected<void, double> { return {5.0}; });

    STATIC_REQUIRE(std::same_as<decltype(result), const helpers::expected<void, double>>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 5.0);
}

} // namespace beman::monadics::tests
