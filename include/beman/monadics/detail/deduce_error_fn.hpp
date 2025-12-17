// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_DEDUCE_ERROR_HPP
#define BEMAN_MONADICS_DETAIL_DEDUCE_ERROR_HPP

#include <beman/monadics/detail/deduce_error_type.hpp>
#include <beman/monadics/detail/instance_of.hpp>
#include <beman/monadics/detail/meta_rebind_error.hpp>

#include <concepts>
#include <utility>

namespace beman::monadics::detail {

namespace _deduce_error_fn {

template <typename Box, typename Traits>
consteval auto deduce() noexcept {
    if constexpr (requires { Traits::error(); }) {
        return []() { return Traits::error(); };
        // return [](auto&&) { return Traits::error(); };
    } else if constexpr (requires { Traits::error(std::declval<Box>()); }) {
        return [](auto&& b) { return Traits::error(std::forward<decltype(b)>(b)); };
    } else if constexpr (requires { std::declval<Box>().error(); }) {
        return [](auto&& b) { return std::forward<decltype(b)>(b).error(); };
    };
};

} // namespace _deduce_error_fn

template <typename Box, typename Traits>
concept has_error_fn = requires {
    requires has_error_type<Box, Traits>;
    requires requires {
        { _deduce_error_fn::deduce<Box, Traits>() } -> std::invocable<Box>;
        {
            _deduce_error_fn::deduce<Box, Traits>()(std::declval<Box>())
        } -> std::convertible_to<deduce_error_type<Box, Traits>>;
    } || requires {
        { _deduce_error_fn::deduce<Box, Traits>() } -> std::invocable;
        { _deduce_error_fn::deduce<Box, Traits>()() } -> std::convertible_to<deduce_error_type<Box, Traits>>;
    };
};

template <typename Box, typename Traits>
    requires has_error_fn<Box, Traits>
inline constexpr auto deduce_error_fn = _deduce_error_fn::deduce<Box, Traits>();

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_DEDUCE_ERROR_HPP
