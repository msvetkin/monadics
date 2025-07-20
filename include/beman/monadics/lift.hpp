// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_LIFT_HPP
#define BEMAN_MONADICS_LIFT_HPP

// #include <beman/monadics/pipe.hpp>
#include <beman/monadics/concepts.hpp>

namespace beman::monadics {

template<typename CPO>
struct pipe : CPO {
  // static inline constexpr CPO cpo{};

  template<typename  T>
  [[nodiscard]] friend constexpr decltype(auto) operator|(T &&t, auto &&cpo) noexcept
    requires requires {
      cpo(std::forward<T>(t));
    }
  {
    return cpo(std::forward<T>(t));
  }
};

[[nodiscard]] decltype(auto) lift_as(Box auto &&box) noexcept {
  return std::forward<decltype(box)>(box);
}

namespace details::_lift {

struct cpo_fn {
  template<typename T>
  [[nodiscard]] constexpr decltype(auto) operator()(T &&t) const noexcept
    requires requires {
      lift_as(std::forward<T>(t));
    }
  {
    return lift_as(std::forward<T>(t));
  }
};

} // namespace details::_lift

inline constexpr pipe<details::_lift::cpo_fn> lift{};

namespace details::_and_then {

struct cpo_fn {
  template<typename T, typename Callback>
  [[nodiscard]] constexpr decltype(auto) operator()(T &&t, Callback &&cb) const noexcept
  {
      decltype(auto) lifted = lift_as(std::forward<T>(t));
      if (!lifted.has_value()) {
        return std::forward<decltype(lifted)>(lifted).error();
      }

      return std::invoke(std::forward<Callback>(cb), std::forward<decltype(lifted)>(lifted));
  }
};

} // namespace details::_and_then

inline constexpr pipe<details::_and_then::cpo_fn> and_then{};

namespace details::_unwrap_or {

struct cpo_fn {
  template<typename T, typename U>
  [[nodiscard]] constexpr decltype(auto) operator()(T &&t, U &&u) const noexcept
  {
      decltype(auto) lifted = lift_as(std::forward<T>(t));
      if (!lifted.has_value()) {
        return std::forward<decltype(lifted)>(lifted).error();
      }

      return std::forward<decltype(lifted)>(lifted).value();
  }
};

} // namespace details::_unwrap_or

inline constexpr pipe<details::_unwrap_or::cpo_fn> unwrap_or{};

}  // namespace beman::monadics

#endif // BEMAN_MONADICS_LIFT_HPP
