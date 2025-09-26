// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_PIPE_HPP
#define BEMAN_MONADICS_DETAIL_PIPE_HPP

#include <utility>

namespace beman::monadics::detail {

template <typename CPO>
struct pipe_for {
    inline static constexpr CPO cpo{};

    template <typename Fn>
    struct action {
        Fn fn;

        template <typename Box>
        [[nodiscard]] inline constexpr decltype(auto) operator()(Box&& box) noexcept
        {
            return cpo(std::forward<Box>(box), std::forward<Fn>(fn));
        }

        template <typename Box, typename A>
            requires(std::same_as<std::remove_cvref_t<A>, action>)
        [[nodiscard]] friend inline constexpr auto operator|(Box&& box, A&& a) noexcept
        {

            return std::forward<A>(a)(std::forward<Box>(box));
        }
    };

    template <typename Fn>
    [[nodiscard]] constexpr decltype(auto) operator()(Fn&& fn) const noexcept {
        return action<decltype(fn)>{std::forward<Fn>(fn)};
    }

    // template <typename Fn>
    // [[nodiscard]] constexpr decltype(auto) operator()() const noexcept {
        // return action<decltype(fn)>{std::forward<Fn>(fn)};
    // }
};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_PIPE_HPP
