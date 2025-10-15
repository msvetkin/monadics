// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "beman/monadics/detail/transform.hpp"

#include <catch2/catch_test_macros.hpp>

#include "helpers/raw-ptr-full.hpp"

namespace beman::monadics::tests {

TEST_CASE("has-box-traits") {
    STATIC_REQUIRE(has_box_traits<int*>);
}

TEST_CASE("with-value") {

  int value{10};
  const auto result = &value | transform([] (auto v) {
      return v * 2;
  });

  /*
  &value | inspect([] (auto &v) { });
  */

  // constexpr int value = [] () {
    // int value{10};
    // int *p = nullptr;
    // // p | or_else(malloc)

    // auto result = p
      // | or_else([&value] () {
        // return &value;
      // });
    // return *result;
  // }();

  // STATIC_REQUIRE(value == 10);
}

} // namespace beman::monadics::tests
