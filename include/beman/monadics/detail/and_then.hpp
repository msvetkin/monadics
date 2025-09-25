// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_AND_THEN_HPP
#define BEMAN_MONADICS_DETAIL_AND_THEN_HPP

#include "beman/monadics/box_traits.hpp"
#include <beman/monadics/detail/pipe.hpp>
#include <type_traits>

namespace beman::monadics {

namespace detail::_and_then {

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

struct op_fn {
    template <typename Box, typename Fn>
    [[nodiscard]] inline constexpr auto operator()(Box &&box, Fn &&fn) const noexcept {
        using BoxTraits = box_traits_for<Box>;
        using NewBox = invoke_result_t<
          decltype(std::forward<Fn>(fn)),
          decltype(BoxTraits::value(std::forward<Box>(box)))
        >;
        using NewBoxTraits = box_traits_for<NewBox>;

        if (BoxTraits::has_value(box)) {
            if constexpr (std::is_void_v<typename BoxTraits::value_type>) {
              return std::forward<Fn>(fn)();
            } else {
              return std::forward<Fn>(fn)(BoxTraits::value(std::forward<Box>(box)));
            }
        }

        if constexpr (requires { BoxTraits::error(std::forward<Box>(box)); }) {
            return NewBoxTraits::lift(BoxTraits::error(std::forward<Box>(box)));
        } else {
            return NewBoxTraits::lift(BoxTraits::error());
        }
    }
};

} // namespace detail::_and_then

inline constexpr detail::pipe_for<detail::_and_then::op_fn> and_then{};

} // namespace beman::monadics

#endif // BEMAN_MONADICS_DETAIL_AND_THEN_HPP
