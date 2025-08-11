// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_AND_THEN_HPP
#define BEMAN_MONADICS_AND_THEN_HPP

#include <beman/monadics/concepts/same_template_as.hpp>
#include <beman/monadics/type_traits/invoke_result.hpp>
#include <beman/monadics/box_traits.hpp>
#include <beman/monadics/pipe.hpp>

#include <concepts>
#include <type_traits>
#include <utility> // std::forward

namespace beman::monadics {

namespace details::_and_then {

template <typename...>
concept False = false;

template <typename Fn, typename Value>
struct inv {
    static_assert(False<Fn, Value>, "Not able to invoke Fn with Value");
};

template <typename Fn, typename Value>
    requires(std::invocable<Fn, Value>)
struct inv<Fn, Value> {
    using type = std::invoke_result_t<Fn, Value>;
};

template <std::invocable Fn>
struct inv<Fn, void> {
    using type = std::invoke_result_t<Fn>;
};

template <typename Fn, typename Value>
using inv_t = typename inv<Fn, Value>::type;

template <typename Box,
          typename Fn,
          typename BoxTraits,
          typename NewBox = inv_t<Fn, decltype(BoxTraits::value(std::declval<Box>()))>>
    requires requires {
        requires same_template_as<Box, NewBox>;
        requires std::same_as<typename BoxTraits::error_type, typename box_traits_for<NewBox>::error_type>;
    }
using and_then_return = std::remove_cvref_t<NewBox>;

template <typename BoxTraits>
concept SupportValue = !std::same_as<typename BoxTraits::value_type, void>;

// template<typename Box, typename BoxTraits>
// concept InvocableWithValue = requires {

// };

// template<typename BoxTraits>
// concept SupportValue = !std::same_as<typename BoxTraits::value_type, void>;

struct op_fn {
    template <typename Box, typename Fn, typename BoxTraits = box_traits_for<Box>
              // typename NewBox = and_then_return<Box, Fn, BoxTraits>
              >
        requires requires { typename and_then_return<Box, Fn, BoxTraits>; }
    [[nodiscard]] inline constexpr auto operator()(Box box,
                                                   Fn  fn) const noexcept -> and_then_return<Box, Fn, BoxTraits> {
        if (!BoxTraits::has_value(box)) {
            if constexpr (BoxTraits::support_error) {
                return {BoxTraits::error(std::forward<Box>(box))};
            } else {
                return {BoxTraits::error()};
            }
        }

        if constexpr (std::same_as<typename BoxTraits::value_type, void>) {
            return std::invoke(std::forward<Fn>(fn));
        } else {
            return std::invoke(std::forward<Fn>(fn), BoxTraits::value(std::forward<Box>(box)));
        }
    }
};

} // namespace details::_and_then

inline constexpr pipe_for<details::_and_then::op_fn> and_then{};

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
