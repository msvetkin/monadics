// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "beman/monadics/detail/transform_error.hpp"

#include "helpers/myexpected.hpp"

#include "catch2/catch_test_macros.hpp"

namespace beman::monadics::tests {

TEST_CASE("has-box-traits") { STATIC_REQUIRE(has_box_traits<helpers::expected<void, int>>); }

TEST_CASE("void-value-return-non-void-value") {
    constexpr auto result = helpers::expected<void, int>{} | transform_error([](int v) { return v * 2.0; });

    STATIC_REQUIRE(std::same_as<decltype(result), const helpers::expected<void, double>>);
    STATIC_REQUIRE(result.has_value());
    // STATIC_REQUIRE(result.value() == 0.0);
}

// TEST_CASE("void-value-return-void-value") {
// constexpr auto result = helpers::expected<void, int>{}
// | transform([] () {
// return;
// });

// STATIC_REQUIRE(std::same_as<decltype(result), const helpers::expected<void, int>>);
// STATIC_REQUIRE(result.has_value() == true);
// }

// TEST_CASE("value-return-different-value") {
// constexpr auto result = helpers::expected<int, double>{10}
// | transform([] (auto) {
// return std::string_view{"some"};
// });

// STATIC_REQUIRE(std::same_as<decltype(result), const helpers::expected<std::string_view, double>>);
// STATIC_REQUIRE(result.has_value());
// STATIC_REQUIRE(result.value() == "some");
// }

// TEST_CASE("value-return-void-value") {
// constexpr auto result = helpers::expected<int, double>{10}
// | transform([] (auto) {
// return;
// });

// STATIC_REQUIRE(std::same_as<decltype(result), const helpers::expected<void, double>>);
// STATIC_REQUIRE(result.has_value());
// }

} // namespace beman::monadics::tests
