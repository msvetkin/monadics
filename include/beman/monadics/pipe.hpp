// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_PIPE_HPP
#define BEMAN_MONADICS_PIPE_HPP

#include <utility> // std::forward, std::declval

namespace beman::monadics {

template<auto Op>
struct pipe_for {
  template<typename Fn>
  struct action {
    Fn fn;

    template<typename Box, typename A>
        requires(std::same_as<std::remove_cvref_t<A>, action>)
    [[nodiscard]] friend inline constexpr auto operator|(Box &&box, A &&a) noexcept
      requires requires {
        Op(std::forward<Box>(box), std::forward<A>(a).fn);
      }
    {
      return Op(std::forward<Box>(box), std::forward<A>(a).fn);
    }
  };

  // template<typename T, typename F>
  // inline static constexpr auto invocable =
      // requires { std::declval<T>() | std::declval<Action<F>>(); };

  template<typename Fn>
  [[nodiscard]] constexpr decltype(auto) operator()(Fn &&fn) const noexcept {
    return action<decltype(fn)>{std::forward<Fn>(fn)};
  }
};

}  // namespace beman::monadics

#endif // BEMAN_MONADICS_PIPE_HPP
