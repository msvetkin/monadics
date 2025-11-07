// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_PIPE_HPP
#define BEMAN_MONADICS_DETAIL_PIPE_HPP

#include <type_traits>
#include <utility>

namespace beman::monadics::detail {

template <typename CPO>
struct pipe_for {
    inline static constexpr CPO cpo{};

    template <typename Fn>
    struct action {
        Fn fn;

        template <typename Box>
        [[nodiscard]] inline constexpr decltype(auto) operator()(Box&& box) noexcept {
            // static_assert(std::same_as<decltype(box), Boo*&&>);
            return cpo(std::forward<Box>(box), std::forward<Fn>(fn));
        }

        template <typename Box, typename A>
            requires(std::same_as<std::remove_cvref_t<A>, action>)
        [[nodiscard]] friend inline constexpr decltype(auto) operator|(Box&& box, A&& a) noexcept {
            // if constexpr (std::is_pointer_v<std::remove_cvref_t<Box>>) {
            // return std::forward<A>(a)(box);
            // } else {
            return std::forward<A>(a)(std::forward<Box>(box));
            // }
            // static_assert(std::same_as<decltype(box), Boo*&&>);
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
