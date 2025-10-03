// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "beman/monadics/detail/and_then.hpp"

#include <catch2/catch_test_macros.hpp>

#include "helpers/optional-full.hpp"

namespace beman::monadics::tests {

TEST_CASE("has-box-traits") {
    STATIC_REQUIRE(has_box_traits<std::optional<int>>);
}

TEST_CASE("with-value") {
  constexpr auto result = std::optional{10}
      | and_then([] (auto &&value) {
          return std::optional{std::forward<decltype(value)>(value) * 2.0};
      });
  STATIC_REQUIRE(std::same_as<decltype(result), const std::optional<double>>);
  STATIC_REQUIRE(result.has_value());
  STATIC_REQUIRE(result.value() == 20.0);
}

TEST_CASE("with-nullopt") {
  constexpr auto result = std::optional<int>{}
      | and_then([] (auto &&value) {
          return std::optional{std::forward<decltype(value)>(value) * 2.0};
      });
  STATIC_REQUIRE(std::same_as<decltype(result), const std::optional<double>>);
  STATIC_REQUIRE(result.has_value() == false);
}

} // namespace beman::monadics::tests
