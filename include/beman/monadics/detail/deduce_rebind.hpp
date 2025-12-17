// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_DEDUCE_REBIND_HPP
#define BEMAN_MONADICS_DETAIL_DEDUCE_REBIND_HPP

#include <beman/monadics/detail/instance_of.hpp>
#include <beman/monadics/detail/meta_rebind_value.hpp>

namespace beman::monadics::detail {

namespace _deduce_rebind {

template <typename Box, typename Traits, typename T>
consteval auto deduce() noexcept {
    if constexpr (requires { typename Traits::template rebind<T>; }) {
        return std::type_identity<Traits>{};
    } else if constexpr (requires { typename Box::template rebind<T>; }) {
        return std::type_identity<Box>{};
    } else if constexpr (requires { get_meta_rebind<Box>(); }) {
        return get_meta_rebind<Box>();
    }
}

} // namespace _deduce_rebind

template <typename Box, typename Traits, typename T>
concept has_rebind = requires {
    { _deduce_rebind::deduce<Box, Traits, T>() } -> instance_of<std::type_identity>;
};

template <typename Box, typename Traits, typename T>
    requires has_rebind<Box, Traits, T>
using deduce_rebind = typename decltype(_deduce_rebind::deduce<Box, Traits, T>())::type;

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_DEDUCE_REBIND_HPP
