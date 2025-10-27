// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_HELPERS_OPTIONAL_FULL_HPP
#define BEMAN_MONADICS_HELPERS_OPTIONAL_FULL_HPP

#include <beman/monadics/box_traits.hpp>

#include <optional>

template <typename T>
struct beman::monadics::box_traits<std::optional<T>> {
    using value_type = T;
    using error_type = std::nullopt_t;

    template <typename V>
    using rebind_value = std::optional<V>;

    template <typename>
    using rebind_error = std::optional<T>;

    // using has_value_t = bool (std::optional<T>::*)() const noexcept;
    // inline static constexpr auto has_value = static_cast<has_value_t>(&std::optional<T>::operator bool);
    [[nodiscard]] inline static constexpr bool has_value(const std::optional<T>& box) noexcept { return box.has_value(); }

    [[nodiscard]] inline static constexpr decltype(auto) value(auto&& box) noexcept {
        return std::forward<decltype(box)>(box).value();
    }

    [[nodiscard]] inline static constexpr auto error() noexcept { return std::nullopt; }

    [[nodiscard]] inline static constexpr std::optional<T> lift(auto &&value) noexcept {
        return {std::forward<decltype(value)>(value)};
    }

    [[nodiscard]] inline static constexpr std::optional<T> lift_error() noexcept {
        return {};
    }
};

/*
template <typename T>
struct beman::monadics::box_traits<std::optional<T>> {
    using value_type = T;
    using error_type = std::nullopt_t;

    template <typename V>
    using rebind_value = std::optional<V>;

    template <typename>
    using rebind_error = std::optional<T>;

    template<typename Box>
    inline static constexpr has_value_fn<Box, T> has_value = &Box::has_value;

    template<typename Box>
    inline static constexpr value_fn<Box, T> value = &Box::value;

    template<typename Box>
    inline static constexpr auto error = std::nullopt;
};

template <is_instance_of<std::optional> Box>
struct beman::monadics::box_traits<Box> {
    using value_type = typename Box::value_type;
    using error_type = std::nullopt_t;

    template <typename V>
    using rebind_value = std::optional<V>;

    template <typename>
    using rebind_error = std::optional<T>;

    template<typename Box>
    inline static constexpr has_value_fn<Box, T> has_value = &Box::has_value;

    template<typename Box>
    inline static constexpr value_fn<Box, T> value = &Box::value;

    template<typename Box>
    inline static constexpr auto error = std::nullopt;
};
*/

#endif // BEMAN_MONADICS_HELPERS_OPTIONAL_FULL_HPP
