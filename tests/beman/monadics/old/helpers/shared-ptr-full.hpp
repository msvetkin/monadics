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

    [[nodiscard]] inline static constexpr bool has_value(const auto& box) noexcept { return static_cast<bool>(box); }

    [[nodiscard]] inline static constexpr decltype(auto) value(auto&& box) noexcept {
        return *std::forward<decltype(box)>(box);
    }

    [[nodiscard]] inline static constexpr auto error() noexcept { return nullptr; }

    [[nodiscard]] inline static constexpr std::shared_ptr<T> lift(auto&& value) noexcept {
        return std::make_shared<T>(std::forward<decltype(value)>(value));
    }

    [[nodiscard]] inline static constexpr std::shared_ptr<T> lift_error() noexcept { return {}; }
};

/*
template <is_instance_of<std::shared_ptr> Box>
struct beman::monadics::box_traits<Box> {
    [[nodiscard]] inline static constexpr bool has_value(const Box& box) noexcept {
      return static_cast<bool>(box);
    }

    [[nodiscard]] inline static constexpr decltype(auto) value(Box&& box) noexcept {
        return *std::forward<decltype(box)>(box);
    }

    [[nodiscard]] inline static constexpr Box lift(auto &&v) noexcept {
      return std::make_shared<typename Box::element_type>(std::forward<V>(v));
    }

    [[nodiscard]] inline static constexpr auto error() noexcept { return nullptr; }
};
error_type is deduced from error fn if error has 0 args.
value_type is deduced as first template argument.
rebind_error is deduced as same type if error fn has 0 args.
rebind_value by default just change first args.
lift_error does not exist, then ctor is used with error().
*/

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
