// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "beman/monadics/detail/and_then.hpp"

#include <catch2/catch_test_macros.hpp>
#include <memory>

#include "helpers/shared-ptr-full.hpp"

namespace beman::monadics::tests {

TEST_CASE("has-box-traits") {
    STATIC_REQUIRE(has_box_traits<std::shared_ptr<int>>);
}

TEST_CASE("with-value") {
  const auto result = std::make_shared<int>(10)
      | and_then([] (auto &&value) {
          return std::make_shared<double>(value * 2.0);
      });
  STATIC_REQUIRE(std::same_as<decltype(result), const std::shared_ptr<double>>);
  REQUIRE(static_cast<bool>(result));
  REQUIRE(*result == 20.0);
}

TEST_CASE("with-nullptr") {
  const auto result = std::shared_ptr<int>{}
      | and_then([] (auto &&value) {
          return std::make_shared<double>(value * 1.0);
      });
  STATIC_REQUIRE(std::same_as<decltype(result), const std::shared_ptr<double>>);
  REQUIRE(static_cast<bool>(result) == false);
}

struct Foo {};

struct Boo {
  std::shared_ptr<Foo> foo;
};

TEST_CASE("with-value-return-member") {
  const auto result = std::make_shared<Boo>()
      | and_then([] (auto &&boo) {
          return std::forward<decltype(boo)>(boo).foo;
      });
  STATIC_REQUIRE(std::same_as<decltype(result), const std::shared_ptr<Foo>>);
  REQUIRE(static_cast<bool>(result) == false);
}

} // namespace beman::monadics::tests
