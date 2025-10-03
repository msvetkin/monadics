// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "beman/monadics/detail/transform.hpp"


#include "helpers/myexpected.hpp"

#include "catch2/catch_test_macros.hpp"


namespace beman::monadics::tests {

TEST_CASE("has-box-traits") {
    STATIC_REQUIRE(has_box_traits<helpers::expected<void, int>>);
}

TEST_CASE("with-void-value") {
  constexpr auto result = helpers::expected<void, int>{}
      | and_then([] () {
          return 2.0;
      });

  STATIC_REQUIRE(result.has_value());
  STATIC_REQUIRE(result.value() == 2.0);
}

// TEST_CASE("without-value") {
  // constexpr auto result = std::optional<int>{}
      // | transform([] (auto &&value) {
          // return value * 2.0;
      // });
  // STATIC_REQUIRE(result.has_value() == false);
// }

// TEST_CASE("shared-ptr-with-value") {
  // auto ptr = std::make_shared<int>(10);

  // const auto result = ptr
      // | transform([] (auto &&value) {
          // return value * 2.0;
      // });

  // REQUIRE(result);
  // REQUIRE(*result == 20.0);
// }

// TEST_CASE("shared-ptr-without-value") {
  // std::shared_ptr<int> ptr;

  // const auto result = ptr
      // | transform([] (auto &&value) {
          // return value * 2.0;
      // });

  // REQUIRE(static_cast<bool>(result) == false);
// }

// TEST_CASE("shared-ptr-rvalue-value") {

  // const auto result = std::make_shared<int>(10)
      // | transform([] (auto &&value) {
          // return value * 2.0;
      // });

  // REQUIRE(static_cast<bool>(result) == true);
// }

// struct Item {
  // std::string name;
  // virtual ~Item() {};
// };

// struct Widget : Item {};

// struct Some {
  // std::shared_ptr<Item> parent;
// };

// TEST_CASE("shared-ptr-parent-child") {
  // const auto parent = [] {
    // auto some = std::make_shared<Some>();
    // some->parent = std::make_shared<Item>();
    // some->parent->name = "test";

    // return std::move(some) | and_then([] (auto &&some) {
        // return some.parent;
    // });
  // }();

  // REQUIRE(parent);
  // REQUIRE(parent->name == "test");
// }


// TEST_CASE("smart-ptr-rvalue-value") {
  // constexpr auto result = [] () {
    // smart_ptr<int> ptr{10};
    // const auto value = ptr.value();
    // return value;
  // }();

  // STATIC_REQUIRE(result == 10);
  // // constexpr smart_ptr<int> ptr{10};

  // // constexpr auto result2 = [] () {
    // // smart_ptr<int> ptr{10};
    // // return ptr | transform([] (auto &&value) {
      // // return value *2.0;
    // // });
  // // }();

  // // STATIC_REQUIRE(result2.has_value());
// }

} // namespace beman::monadics::tests
