// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_PIPE_HPP
#define BEMAN_MONADICS_PIPE_HPP

#include <utility> // std::forward, std::declval

namespace beman::monadics {

template<typename CPO>
struct pipe {
  template<typename Callback>
  struct Action {
    Callback callback;

    template<typename T>
    [[nodiscard]] friend constexpr auto operator|(T &&t, Action &&a) noexcept
      requires requires {
        CPO{}(std::forward<T>(t), std::declval<Callback>());
      }
    {
      return CPO{}(std::forward<T>(t), std::forward<Callback>(a.callback));
    }
  };

  template<typename T, typename F>
  inline static constexpr auto invocable =
      requires { std::declval<T>() | std::declval<Action<F>>(); };

  template<typename Callback>
  [[nodiscard]] constexpr decltype(auto) operator()(
      Callback &&cb) const noexcept {
    return Action<Callback>{std::forward<Callback>(cb)};
  }
};

}  // namespace beman::monadics

#endif // BEMAN_MONADICS_PIPE_HPP
