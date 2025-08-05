// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_TESTS_STD_HPP
#define BEMAN_MONADICS_TESTS_STD_HPP

#include <beman/monadics/box_traits.hpp>
#include <beman/monadics/concepts/instance_of.hpp>

#include <optional>
#include <memory>
#include <type_traits>

template <beman::monadics::instance_of<std::optional> T>
struct beman::monadics::box_traits<T> {
    inline static constexpr bool has_value(const auto& box) noexcept { return static_cast<bool>(box); }

    inline static constexpr decltype(auto) value(auto&& box) noexcept {
        return std::forward<decltype(box)>(box).value();
    }

    inline static constexpr decltype(auto) error() noexcept { return std::nullopt; }
};


namespace my {

template<typename T>
struct Optional {
};

template <beman::monadics::instance_of<std::optional> T>
auto as_box(T) -> T {
  return {
    .value = &T::value,
    .has_value = &T::operator bool,
    .error = std::nullopt
  };
}

// T&&::value; T::value() &&
// T&::value; T::value() &
// const T&::value; T::value() const &
// const T&&::value; T::value() const &&

};

template <beman::monadics::instance_of<std::shared_ptr> T>
struct beman::monadics::box_traits<T> {
    inline static constexpr bool has_value(const auto& box) noexcept { return static_cast<bool>(box); }

    inline static constexpr decltype(auto) value(auto&& box) noexcept { return *std::forward<decltype(box)>(box); }

    template<typename U>
    inline static constexpr decltype(auto) lift(U&& value) noexcept {
      return std::make_shared<std::remove_cvref_t<U>>(std::forward<U>(value));
    }

    inline static constexpr decltype(auto) error() noexcept { return nullptr; }
};

/*
template <beman::monadics::instance_of<std::expected> T>
struct beman::monadics::box_traits<T> {
    inline static constexpr bool has_value(const auto& box) noexcept { return static_cast<bool>(box); }

    inline static constexpr decltype(auto) value(auto&& box) noexcept {
      return std::forward<decltype(box)>(box).value();
    }

    inline static constexpr decltype(auto) error(auto&& box) noexcept {
      return std::forward<decltype(box)>(box).error();
    }
};
*/


#endif // BEMAN_MONADICS_TESTS_STD_HPP
