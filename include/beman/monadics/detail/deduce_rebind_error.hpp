// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_DEDUCE_REBIND_ERROR_HPP
#define BEMAN_MONADICS_DETAIL_DEDUCE_REBIND_ERROR_HPP

#include <beman/monadics/detail/instance_of.hpp>
#include <beman/monadics/detail/meta_rebind_error.hpp>

namespace beman::monadics::detail {

namespace _deduce_rebind_error {

template <typename Box>
struct no_rebind_error {
    template <typename>
    using rebind_error = Box;
};

template <typename Box, typename Traits, typename E>
consteval auto deduce() noexcept {
    if constexpr (requires { typename Traits::template rebind_error<E>; }) {
        return std::type_identity<Traits>{};
    } else if constexpr (requires { typename Box::template rebind_error<E>; }) {
        return std::type_identity<Box>{};
    } else if constexpr (requires { Traits::error(); }) {
        return std::type_identity<no_rebind_error<Box>>{};
    } else if constexpr (requires { get_meta_rebind_error<Box>(); }) {
        return get_meta_rebind_error<Box>();
    } else {
        // static_assert(False<Box>, "I was not able to identify rebind alias, pleases read the docs");
    }
}

} // namespace _deduce_rebind_error

template <typename Box, typename Traits, typename E>
concept has_rebind_error = requires {
    { _deduce_rebind_error::deduce<Box, Traits, E>() } -> instance_of<std::type_identity>;
};

template <typename Box, typename Traits, typename E>
    requires has_rebind_error<Box, Traits, E>
using deduce_rebind_error = typename decltype(_deduce_rebind_error::deduce<Box, Traits, E>())::type;

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_DEDUCE_REBIND_ERROR_HPP
