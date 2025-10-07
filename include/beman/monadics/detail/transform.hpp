// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_TRANSFORM_HPP
#define BEMAN_MONADICS_DETAIL_TRANSFORM_HPP

#include "beman/monadics/box_traits.hpp"
#include <beman/monadics/detail/and_then.hpp>
#include <type_traits>
#include <utility>

namespace beman::monadics {

namespace detail::_transform {

template <typename Fn, typename Value>
consteval auto invoke_result() {
    if constexpr (std::is_void_v<Value>) {
        return std::type_identity<std::invoke_result_t<Fn>>{};
    } else {
        return std::type_identity<std::invoke_result_t<Fn, Value>>{};
    }
}

template <typename Fn, typename Value>
  requires requires {
      requires std::is_void_v<Value>;
      requires std::invocable<Fn>;
  } || requires {
      requires std::invocable<Fn, Value>;
  }
using invoke_result_t = decltype(invoke_result<Fn, Value>())::type;


template<typename Fn, typename NewBoxTraits>
struct LiftedFn {
  Fn fn;

  [[nodiscard]] inline constexpr decltype(auto) operator()(auto &&...v) const noexcept
  {
      if constexpr (std::is_void_v<typename NewBoxTraits::value_type>) {
        std::forward<Fn>(fn)(std::forward<decltype(v)>(v)...);
        return NewBoxTraits::lift();
      } else {
        return NewBoxTraits::lift(std::forward<Fn>(fn)(std::forward<decltype(v)>(v)...));
      }
  }
};

struct op_fn {
    template <typename Box, typename Fn>
    [[nodiscard]] inline constexpr auto operator()(Box &&box, Fn &&fn) const noexcept {
        using BoxTraits = box_traits_for<Box>;
        using NewValue = invoke_result_t<
          decltype(std::forward<Fn>(fn)),
          decltype(BoxTraits::value(std::forward<Box>(box)))
        >;
        using NewBoxTraits = box_traits_for<typename BoxTraits::template rebind_value<NewValue>>;

        return std::forward<Box>(box) | and_then(LiftedFn<decltype(fn), NewBoxTraits>{std::forward<Fn>(fn)});
    }
};

} // namespace detail::_transform

inline constexpr detail::pipe_for<detail::_transform::op_fn> transform{};

} // namespace beman::monadics

#endif // BEMAN_MONADICS_DETAIL_TRANSFORM_HPP
