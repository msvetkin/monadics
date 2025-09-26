// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "beman/monadics/detail/transform.hpp"
#include "catch2/catch_test_macros.hpp"

#include <catch2/catch_template_test_macros.hpp>

#include <optional>

template <typename T>
struct beman::monadics::box_traits<std::optional<T>> {
    using value_type = T;
    using error_type = std::nullopt_t;

    template <typename V>
    using rebind_value = std::optional<V>;

    template <typename>
    using rebind_error = std::optional<T>;

    [[nodiscard]] inline static constexpr bool has_value(const auto& box) noexcept { return box.has_value(); }

    [[nodiscard]] inline static constexpr decltype(auto) value(auto&& box) noexcept {
        return std::forward<decltype(box)>(box).value();
    }

    [[nodiscard]] inline static constexpr auto error() noexcept { return std::nullopt; }

    [[nodiscard]] inline static constexpr std::optional<T> lift(auto &&value) noexcept {
      if constexpr (std::same_as<std::remove_cvref_t<decltype(value)>, std::nullopt_t>) {
          return {};
      } else {
          return {std::forward<decltype(value)>(value)};
      }
    }
};

namespace beman::monadics::tests {

TEST_CASE("has-box-traits") {
    STATIC_REQUIRE(has_box_traits<std::optional<int>>);
}

TEST_CASE("with-value") {
  constexpr auto result = std::optional{10}
      | transform([] (auto &&value) {
          return value * 2.0;
      });

  STATIC_REQUIRE(result.has_value());
  STATIC_REQUIRE(result.value() == 20.0);
}

TEST_CASE("without-value") {
  constexpr auto result = std::optional<int>{}
      | transform([] (auto &&value) {
          return value * 2.0;
      });
  STATIC_REQUIRE(result.has_value() == false);
}

} // namespace beman::monadics::tests
