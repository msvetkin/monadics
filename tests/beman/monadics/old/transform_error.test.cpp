// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "beman/monadics/detail/transform_error.hpp"

#include "catch2/catch_test_macros.hpp"
#include "helpers/optional-full.hpp"
#include "helpers/shared-ptr-full.hpp"
#include "helpers/myexpected.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <utility>

// TODO concepts
//  - and_thenable
//  - or_elsable
//  - transformable
//  - error_transformable

template <typename Box>
concept error_transformable = requires {
    {
        Box{} | transform_error([](auto& e) { return 1; })
    };
};

namespace beman::monadics::tests {

TEST_CASE("optional-has-box-traits") {
    STATIC_REQUIRE(has_box_traits<std::optional<int>>);

    STATIC_REQUIRE(error_transformable<std::shared_ptr<int>> == false);
    STATIC_REQUIRE(error_transformable<std::optional<int>> == false);
}

TEST_CASE("my-expected-with-value") {
    constexpr auto result =
        helpers::expected<int, float>{10} | transform_error([](float&&) { return std::string_view{"hm"}; });

    STATIC_REQUIRE(result.has_value());
    STATIC_REQUIRE(result.value() == 10);
}

TEST_CASE("my-expected-without-value") {
    constexpr auto result = helpers::expected<int, double>{1.0} | transform_error([](auto&& e) { return e + 5.0; });

    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 6.0);
}

TEST_CASE("my-expected-void-with-value") {
    constexpr auto result = helpers::expected<void, float>{} | transform_error([](float&& e) { return e + 10; });

    STATIC_REQUIRE(result.has_value());
}

TEST_CASE("my-expected-void-without-value") {
    constexpr auto result =
        helpers::expected<void, double>{10.0} | transform_error([](double&& e) { return e + 55.5; });

    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 65.5);
}

} // namespace beman::monadics::tests
