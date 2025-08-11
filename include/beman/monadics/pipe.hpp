// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_PIPE_HPP
#define BEMAN_MONADICS_PIPE_HPP

#include <utility> // std::forward, std::declval

namespace beman::monadics {

template <typename Op>
struct pipe_for {
    inline static constexpr Op cpo{};

    template <typename Fn>
    struct action {
        Fn fn;

        template <typename Box>
        [[nodiscard]] inline constexpr decltype(auto) operator()(Box&& box) noexcept
            requires requires { cpo(std::forward<Box>(box), std::forward<Fn>(fn)); }
        {
            using B = decltype(std::forward<Box>(box));
            using F = decltype(std::forward<Fn>(fn));

            return cpo.template operator()<B, F>(std::forward<Box>(box), std::forward<Fn>(fn));
        }

        template <typename Box, typename A>
            requires(std::same_as<std::remove_cvref_t<A>, action>)
        [[nodiscard]] friend inline constexpr auto operator|(Box&& box, A&& a) noexcept
            requires requires { cpo(std::forward<Box>(box), std::forward<A>(a).fn); }
        {

            return std::forward<A>(a)(std::forward<Box>(box));

            // return Op(std::forward<Box>(box), std::forward<A>(a).fn);
        }
    };

    // template<typename T, typename F>
    // inline static constexpr auto invocable =
    // requires { std::declval<T>() | std::declval<Action<F>>(); };

    template <typename Fn>
    [[nodiscard]] constexpr decltype(auto) operator()(Fn&& fn) const noexcept {
        return action<decltype(fn)>{std::forward<Fn>(fn)};
    }
};

} // namespace beman::monadics

#endif // BEMAN_MONADICS_PIPE_HPP
