// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_HELPERS_SHARED_PTR_FULL_HPP
#define BEMAN_MONADICS_HELPERS_SHARED_PTR_FULL_HPP

#include <beman/monadics/box_traits.hpp>

#include <memory>

template <typename T>
struct beman::monadics::box_traits<std::shared_ptr<T>> {
    using value_type = T;
    using error_type = std::nullptr_t;

    template <typename V>
    using rebind_value = std::shared_ptr<V>;

    template <typename>
    using rebind_error = std::shared_ptr<T>;

    [[nodiscard]] inline static constexpr bool has_value(const auto& box) noexcept {
      return static_cast<bool>(box);
    }

    [[nodiscard]] inline static constexpr decltype(auto) value(auto&& box) noexcept {
        return *std::forward<decltype(box)>(box);
    }

    [[nodiscard]] inline static constexpr auto error() noexcept { return nullptr; }

    // lift
    [[nodiscard]] inline static constexpr std::shared_ptr<T> lift(auto &&value) noexcept {
      if constexpr (std::same_as<std::remove_cvref_t<decltype(value)>, std::nullptr_t>) {
          return {};
      } else {
          return std::make_shared<T>(std::forward<decltype(value)>(value));
      }
    }
};

/*
template <typename T>
struct beman::monadics::box_traits<std::shared_ptr<T>> {
    using value_type = T;
    using error_type = std::nullptr_t;

    template <typename V>
    using rebind_value = std::shared_ptr<V>;

    template <typename>
    using rebind_error = std::shared_ptr<T>;

    template<typename Box>
    inline static constexpr has_value_fn<Box, T> has_value = &Box::operator bool;

    template<typename Box>
    inline static constexpr value_fn<Box, T> value = &Box::get;

    template<typename Box>
    inline static constexpr auto error = nullptr;

    template<typename Value>
    inline static constexpr auto error = nullptr;
};

template <is_instance_of<shared_ptr> Box>
struct beman::monadics::box_traits<Box> {
    using value_type = typename Box::element_type;
    using error_type = std::nullptr_t;

    template <typename V>
    using rebind_value = std::shared_ptr<V>;

    template <typename>
    using rebind_error = std::shared_ptr<T>;

    template<typename Box>
    inline static constexpr has_value_fn<Box, T> has_value = &Box::operator bool;

    template<typename Box>
    inline static constexpr value_fn<Box, T> value = &Box::get;

    template<typename Box>
    inline static constexpr auto error = nullptr;
};

template <is_instance_of<shared_ptr> Box>
struct beman::monadics::box_traits<Box> {
    template<typename Box>
    inline static constexpr has_value_fn<Box, T> has_value = &Box::operator bool;

    template<typename Box>
    inline static constexpr value_fn<Box, T> value = &Box::get;

    template<typename Box>
    inline static constexpr auto error = nullptr;
};

template<is_instance_of<shared_ptr> Box>
auto box_as() {
  return box_traits<Box>{
    .has_value = &Box::operator bool,
    .value = &Box::get,
    .error = nullptr
  }
}

template <is_instance_of<shared_ptr> Box>
struct beman::monadics::box_traits<Box> {
    template<typename Box>
    inline static constexpr has_value_fn<Box, T> has_value = &Box::operator bool;

    template<typename Box>
    inline static constexpr value_fn<Box, T> value = &Box::get;

    template<typename Box>
    inline static constexpr auto error = nullptr;
};

*/

#endif // BEMAN_MONADICS_HELPERS_SHARED_PTR_FULL_HPP
