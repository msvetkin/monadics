// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_TRANSFORM_HPP
#define BEMAN_MONADICS_TRANSFORM_HPP

#include "beman/monadics/box.hpp"

#include <type_traits> // std::decay_t
#include <utility> // std::forward

namespace beman::monadics {

namespace details::_transform {

struct fn {

  template<typename Fn>
  struct action {

    template<typename T, typename A>
        requires (std::same_as<std::remove_cvref_t<A>, action>)
    [[nodiscard]] friend inline constexpr decltype(auto) operator|(T &&box, A &&a) noexcept {
      using Traits = box_traits<std::decay_t<T>>;
      using NewValue = std::invoke_result_t<
        decltype(std::forward<Fn>(a.fn)),
        decltype(Traits::value(std::forward<T>(box)))
      >;
      using NewBox = Traits::template rebind<NewValue>;

      if (!Traits::has_value(box)) {
        return NewBox{std::forward<T>(box)};
      }

      return NewBox{std::invoke(
          std::forward<Fn>(a.fn),
          Traits::value(std::forward<T>(box))
      )};
    }

    Fn fn;
  };

  template<typename Fn>
    [[nodiscard]] constexpr inline decltype(auto) operator()(Fn &&fn) const noexcept {
      return action<decltype(std::forward<Fn>(fn))>{std::forward<Fn>(fn)};
  }
};

} // namespace details::_transform

inline constexpr details::_transform::fn transform{};

} // namespace beman::monadics

#endif // BEMAN_MONADICS_TRANSFORM_HPP
