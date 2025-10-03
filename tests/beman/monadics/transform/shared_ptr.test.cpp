// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "beman/monadics/detail/transform.hpp"

#include <catch2/catch_test_macros.hpp>
#include <memory>

#include "helpers/shared-ptr-full.hpp"

namespace beman::monadics::tests {

TEST_CASE("has-box-traits") {
    STATIC_REQUIRE(has_box_traits<std::shared_ptr<int>>);
}

TEST_CASE("with-value") {
  const auto result = std::make_shared<int>(10)
      | transform([] (auto &&value) {
          return value * 2.0;
      });
  STATIC_REQUIRE(std::same_as<decltype(result), const std::shared_ptr<double>>);
  REQUIRE(static_cast<bool>(result));
  REQUIRE(*result == 20.0);
}

TEST_CASE("with-nullptr") {
  const auto result = std::shared_ptr<int>{}
      | transform([] (auto &&value) {
          return value * 1.0;
      });
  STATIC_REQUIRE(std::same_as<decltype(result), const std::shared_ptr<double>>);
  REQUIRE(static_cast<bool>(result) == false);
}


TEST_CASE("with-value-return-member") {
  struct Boo {
    int value;
  };

  const auto result = std::make_shared<Boo>(10)
      | transform([] (auto &&boo) {
          return std::forward<decltype(boo)>(boo).value;
      });
  STATIC_REQUIRE(std::same_as<decltype(result), const std::shared_ptr<int>>);
  REQUIRE(static_cast<bool>(result));
  REQUIRE(*result == 10);
}

} // namespace beman::monadics::tests
