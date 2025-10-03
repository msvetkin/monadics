// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "beman/monadics/detail/or_else.hpp"

#include "helpers/optional-full.hpp"
#include "helpers/shared-ptr-full.hpp"

#include <catch2/catch_template_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("optional-has-box-traits") {
    STATIC_REQUIRE(has_box_traits<std::optional<int>>);
}

TEST_CASE("with-value") {
  constexpr auto result = std::optional{10}
      | or_else([] () {
          return std::optional{2.0};
      });

  STATIC_REQUIRE(result.has_value());
  STATIC_REQUIRE(result.value() == 10);
}

TEST_CASE("without-value") {
  constexpr auto result = std::optional<int>{}
      | or_else([] () {
          return std::optional{2.0};
      });
  STATIC_REQUIRE(result.has_value() == true);
  STATIC_REQUIRE(result.value() == 2.0);
}

TEST_CASE("shared-ptr-with-value") {
  auto ptr = std::make_shared<int>(10);

  const auto result = ptr
      | or_else([] () {
          return std::make_shared<double>(2.0);
      });

  REQUIRE(result);
  REQUIRE(*result == 10.0);
}

TEST_CASE("shared-ptr-without-value") {
  std::shared_ptr<int> ptr;

  const auto result = ptr
      | or_else([] () {
          return std::make_shared<double>(2.0);
      });

  REQUIRE(static_cast<bool>(result) == true);
  REQUIRE(*result == 2.0);
}

TEST_CASE("shared-ptr-rvalue-value") {
  const auto result = std::make_shared<int>(10)
      | or_else([] () {
          return std::shared_ptr<double>{};
      });

  REQUIRE(static_cast<bool>(result) == true);
}

} // namespace beman::monadics::tests
