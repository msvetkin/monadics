// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_DEDUCE_VALUE_TYPE_HPP
#define BEMAN_MONADICS_DETAIL_DEDUCE_VALUE_TYPE_HPP

#include <beman/monadics/detail/instance_of.hpp>
#include <beman/monadics/detail/meta_extract_value_type.hpp>

namespace beman::monadics::detail {

namespace _deduce_value_type {

template <typename Box, typename Trait>
[[nodiscard]] consteval auto deduce() noexcept {
    if constexpr (requires { typename Trait::value_type; }) {
        return std::type_identity<typename Trait::value_type>{};
    } else if constexpr (requires { typename Box::value_type; }) {
        return std::type_identity<typename Box::value_type>{};
    } else if constexpr (requires { meta_extract_value_type<Box>(); }) {
        return meta_extract_value_type<Box>();
    }
}

} // namespace _deduce_value_type

template <typename Box, typename Traits>
concept has_value_type = requires {
    { _deduce_value_type::deduce<Box, Traits>() } -> instance_of<std::type_identity>;
};

template <typename Box, typename Traits>
    requires has_value_type<Box, Traits>
using deduce_value_type = typename decltype(_deduce_value_type::deduce<Box, Traits>())::type;

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_DEDUCE_VALUE_TYPE_HPP
