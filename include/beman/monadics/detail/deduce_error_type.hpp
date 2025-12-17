// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_DEDUCE_ERROR_TYPE_HPP
#define BEMAN_MONADICS_DETAIL_DEDUCE_ERROR_TYPE_HPP

#include <beman/monadics/detail/instance_of.hpp>

namespace beman::monadics::detail {

namespace _deduce_error_type {

template <typename Box, typename Trait>
consteval auto deduce() noexcept {
    if constexpr (requires { typename Trait::error_type; }) {
        return std::type_identity<typename Trait::error_type>{};
    } else if constexpr (requires { typename Box::error_type; }) {
        return std::type_identity<typename Box::error_type>{};
    } else if constexpr (requires { Trait::error(); }) {
        return std::type_identity<std::remove_cvref_t<decltype(Trait::error())>>{};
    }
}

} // namespace _deduce_error_type

template <typename Box, typename Traits>
concept has_error_type = requires {
    { _deduce_error_type::deduce<Box, Traits>() } -> instance_of<std::type_identity>;
};

template <typename Box, typename Traits>
    requires has_error_type<Box, Traits>
using deduce_error_type = typename decltype(_deduce_error_type::deduce<Box, Traits>())::type;

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_DEDUCE_ERROR_TYPE_HPP
