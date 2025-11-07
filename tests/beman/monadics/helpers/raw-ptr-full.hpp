// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_HELPERS_RAW_PTR_FULL_HPP
#define BEMAN_MONADICS_HELPERS_RAW_PTR_FULL_HPP

#include <beman/monadics/box_traits.hpp>

#ifndef BEMAN_MONADICS_BOX_TRAITS_BUILDER
template <typename T>
struct beman::monadics::box_traits<T*> {
    using value_type = T;
    using error_type = std::nullptr_t;

    template <typename V>
    using rebind_value = V*;

    template <typename>
    using rebind_error = T*;

    [[nodiscard]] inline static constexpr bool has_value(const auto& box) noexcept { return static_cast<bool>(box); }

    [[nodiscard]] inline static constexpr decltype(auto) value(auto&& box) noexcept {
        // static_assert(std::same_as<decltype(box), Boo*&&>);
        return std::forward<decltype(box)>(box);
    }

    [[nodiscard]] inline static constexpr auto error() noexcept { return nullptr; }

    // lift
    [[nodiscard]] inline static constexpr decltype(auto) lift(auto) noexcept { return static_cast<T*>(nullptr); }

    [[nodiscard]] inline static constexpr decltype(auto) lift_error() noexcept { return static_cast<T*>(nullptr); }
};

#else

template <typename Box>
    requires std::is_pointer_v<Box>
struct beman::monadics::box_traits<Box> {
    using value_type = std::remove_pointer_t<Box>;

    template <typename V>
    using rebind_value = V*;

    template <typename>
    using rebind_error = Box;

    [[nodiscard]] inline static constexpr bool has_value(const Box& box) noexcept { return static_cast<bool>(box); }

    template <typename B = Box>
    [[nodiscard]] inline static constexpr decltype(auto) value(B&& box) noexcept {
        return *std::forward<B>(box);
    }

    [[nodiscard]] inline static constexpr auto error() noexcept { return nullptr; }

    // lift
    [[nodiscard]] inline static constexpr Box lift(auto) noexcept { return nullptr; }

    [[nodiscard]] inline static constexpr Box lift_error() noexcept { return nullptr; }
};

#endif

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

#endif // BEMAN_MONADICS_HELPERS_RAW_PTR_FULL_HPP
