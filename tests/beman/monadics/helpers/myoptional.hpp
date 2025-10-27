// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_TESTS_HELPERS_MYOPTIONAL_HPP
#define BEMAN_MONADICS_TESTS_HELPERS_MYOPTIONAL_HPP

#include <beman/monadics/box_traits.hpp>

namespace helpers {

struct none {};

template <typename T>
struct myoptional {
  bool has_value{false};
  T value;
};

} // namespace helpers

template <typename T>
struct beman::monadics::box_traits<helpers::myoptional<T>> {
    using value_type = T;
    using error_type = helpers::none;

    template <typename V>
    using rebind_value = helpers::myoptional<V>;

    template <typename>
    using rebind_error = helpers::myoptional<T>;

    [[nodiscard]] inline static constexpr bool has_value(const auto& box) noexcept { return box.has_value; }

    [[nodiscard]] inline static constexpr decltype(auto) value(auto&& box) noexcept {
        return std::forward<decltype(box)>(box).value;
    }

    [[nodiscard]] inline static constexpr helpers::none error() noexcept { return {}; }

    [[nodiscard]] inline static constexpr helpers::myoptional<T> lift(auto &&v) noexcept {
      return {.has_value = true, .value = std::forward<decltype(value)>(value)};
    }

    [[nodiscard]] inline static constexpr helpers::myoptional<T> lift_error() noexcept {
      return {};
    }
};

#endif  // BEMAN_MONADICS_TESTS_HELPERS_MYOPTIONAL_HPP
