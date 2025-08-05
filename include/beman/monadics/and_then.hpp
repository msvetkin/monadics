// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_AND_THEN_HPP
#define BEMAN_MONADICS_AND_THEN_HPP

#include <beman/monadics/concepts/same_template_as.hpp>
#include <beman/monadics/type_traits/invoke_result.hpp>
#include <beman/monadics/box_traits.hpp>
#include <beman/monadics/pipe.hpp>

#include <type_traits> // std::decay_t
#include <utility>     // std::forward

namespace beman::monadics {

namespace details::_and_then {

template <typename Box, typename F, typename Traits,
          typename R = invoke_result_t<F, decltype(Traits::value(std::declval<Box>()))>>
    requires requires {
        requires same_template_as<Box, R>;
        // requires std::same_as<
        // typename std::remove_cvref_t<T>::error_type,
        // typename std::remove_cvref_t<R>::error_type>;
    }
using and_then_return = R;

struct op_fn {
  template<typename Box, typename Fn>
  [[nodiscard]] inline constexpr auto operator()(Box&& box, Fn&& fn) const noexcept -> and_then_return<decltype(box), decltype(fn), box_traits_for<Box>> {
      using Traits = box_traits_for<Box>;

      if (!Traits::has_value(box)) {
          if constexpr (Traits::support_error) {
              return Traits::error(std::forward<Box>(box));
          } else {
              return Traits::error();
          }
      }

      return std::invoke(std::forward<Fn>(fn), Traits::value(std::forward<Box>(box)));
  }
};

} // namespace details::_and_then

// inline constexpr details::_and_then::fn and_then{};

inline constexpr pipe_for<details::_and_then::op_fn{}> and_then{};

} // namespace beman::monadics

/*

  Box

  M<T>    -> T -> transform       -> U    -> M<T> rebind U -> M<U>
  value
  error
  has_value
  rebind


  M<T>    -> T -> and_then        -> M<U>
  M<T>    -> T -> or_else         -> M<U>


  M<T, E> -> E -> transform_error -> U    -> M<T, U>

  // fp(box, success, fail)

  auto transform = box
    | unwrap
    | callback
    | lift

  using U = ... value or monad
  using BoxTraits = box_traits<Box>;
  using NewBox = BoxTraits::rebind<T>

  if (!Traits::has_value(box)) {
    return NewBox<Value>{std::nullopt};
  }

  return NewBox<Value>{Traits::value(std::forward<Box>(box))};


*/

#endif // BEMAN_MONADICS_AND_THEN_HPP
