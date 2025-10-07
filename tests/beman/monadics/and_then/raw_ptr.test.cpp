// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "beman/monadics/detail/and_then.hpp"

#include <catch2/catch_test_macros.hpp>

#include "helpers/raw-ptr-full.hpp"

namespace beman::monadics::tests {

TEST_CASE("has-box-traits") {
    STATIC_REQUIRE(has_box_traits<int*>);
}

struct Foo {
  int value{};
};
struct Boo {
  Foo *foo{nullptr};
};

TEST_CASE("with-value") {

  constexpr int value = [] () {
    Foo foo{.value = 5};
    Boo boo{.foo = &foo};

    auto result = &boo
      | and_then([] (auto &&b) {
        return b.foo;
      })
      | and_then([] (auto &&f) {
        f.value += 5;
        return &f;
      });
    return result->value;
  }();

  STATIC_REQUIRE(value == 10);
}

} // namespace beman::monadics::tests
