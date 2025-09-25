// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "beman/monadics/detail/and_then.hpp"

#include "helpers/myoptional.hpp"

#include <catch2/catch_template_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("has-box-traits") {
    STATIC_REQUIRE(has_box_traits<helpers::myoptional<int>>);
}

TEST_CASE("myoptional-with-value") {
  constexpr auto result = helpers::myoptional<int>{.has_value = true, .value = 10}
      | and_then([] (auto &&value) {
          return helpers::myoptional<double>{
            .has_value = true,
            .value = std::forward<decltype(value)>(value) * 2.0
          };
      });
  STATIC_REQUIRE(result.has_value);
  STATIC_REQUIRE(result.value == 20.0);
}

TEST_CASE("myoptional-with-error") {
  constexpr auto result = helpers::myoptional<int>{.has_value = false, .value = 10}
      | and_then([] (auto &&value) {
          return helpers::myoptional<double>{
            .has_value = true,
            .value = std::forward<decltype(value)>(value) * 2.0
          };
      });
  STATIC_REQUIRE(result.has_value == false);
}

} // namespace beman::monadics::tests
