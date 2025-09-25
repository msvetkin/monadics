// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "beman/monadics/detail/transform.hpp"

#include "helpers/myoptional.hpp"

#include <catch2/catch_template_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("has-box-traits") {
    STATIC_REQUIRE(has_box_traits<helpers::myoptional<int>>);
}

TEST_CASE("") {
  constexpr auto result = helpers::myoptional<int>{.has_value = true, .value = 10}
      | transform([] (auto &&value) {
          return value * 2.0;
      });
}

} // namespace beman::monadics::tests
