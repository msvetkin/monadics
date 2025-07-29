// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_OR_ELSE
#define BEMAN_MONADICS_OR_ELSE

#include <beman/monadics/concepts/same_template_as.hpp>
#include <beman/monadics/type_traits/invoke_result.hpp>
#include "beman/monadics/type_traits/rebind_value.hpp"
#include <beman/monadics/box_traits.hpp>

#include <type_traits> // std::decay_t
#include <utility>     // std::forward

namespace beman::monadics {

namespace details::_or_else {

template <typename Box, typename F, typename R = std::invoke_result_t<F> >
    requires requires { requires same_template_as<Box, R>; }
using or_else_return = rebind_value<Box, R>;

struct fn {
    template <typename Fn>
    struct action {
        template <typename Box, typename A>
            requires(std::same_as<std::remove_cvref_t<A>, action>)
        [[nodiscard]] friend inline constexpr auto
        operator|(Box&& box, A&& a) noexcept -> or_else_return<decltype(box), decltype(a.fn)> {
            using Traits = box_traits_for<Box>;

            if (Traits::has_value(box)) {
                return Traits::value(std::forward<Box>(box));
            }

            if constexpr (Traits::support_error) {
                return std::invoke(std::forward<Fn>(a.fn), Traits::error(std::forward<Box>(box)));
            } else {
                return std::invoke(std::forward<Fn>(a.fn));
            }
        }

        Fn fn;
    };

    template <typename Fn>
    [[nodiscard]] constexpr inline decltype(auto) operator()(Fn&& fn) const noexcept {
        return action<decltype(std::forward<Fn>(fn))>{std::forward<Fn>(fn)};
    }
};

} // namespace details::_or_else

inline constexpr details::_or_else::fn or_else{};

} // namespace beman::monadics

#endif // BEMAN_MONADICS_OR_ELSE
