// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_AND_THEN_HPP
#define BEMAN_MONADICS_AND_THEN_HPP

#include <beman/monadics/concepts/same_template_as.hpp>
#include <beman/monadics/type_traits/invoke_result.hpp>
#include <beman/monadics/box_traits.hpp>

#include <type_traits> // std::decay_t
#include <utility>     // std::forward

namespace beman::monadics {

namespace details::_and_then {

template <typename Box, typename F, typename R = invoke_result_t<F, decltype(std::declval<Box>().value())>>
    requires requires {
        requires same_template_as<Box, R>;
        // requires std::same_as<
        // typename std::remove_cvref_t<T>::error_type,
        // typename std::remove_cvref_t<R>::error_type>;
    }
using and_then_return = R;

struct fn {
    template <typename Fn>
    struct action {
        template <typename Box, typename A>
            requires(std::same_as<std::remove_cvref_t<A>, action>)
        [[nodiscard]] friend inline constexpr auto
        operator|(Box&& box, A&& a) noexcept -> and_then_return<decltype(box), decltype(a.fn)> {
            using Traits = box_traits_for<Box>;

            if (!Traits::has_value(box)) {
                if constexpr (Traits::support_error) {
                    return Traits::error(std::forward<Box>(box));
                } else {
                    return Traits::error();
                }
            }

            return std::invoke(std::forward<Fn>(a.fn), Traits::value(std::forward<Box>(box)));
        }

        Fn fn;
    };

    template <typename Fn>
    [[nodiscard]] constexpr inline decltype(auto) operator()(Fn&& fn) const noexcept {
        return action<decltype(std::forward<Fn>(fn))>{std::forward<Fn>(fn)};
    }
};

} // namespace details::_and_then

inline constexpr details::_and_then::fn and_then{};

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
