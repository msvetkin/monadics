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

struct op_fn {
    template <typename Box, typename Fn>
    [[nodiscard]] inline constexpr auto operator()(Box &&box, Fn &&fn) const noexcept {
        using BoxTraits = box_traits_for<Box>;
        using NewValue = invoke_result_t<
          decltype(std::forward<Fn>(fn)),
          decltype(BoxTraits::value(std::forward<Box>(box)))
        >;
        using NewBoxTraits = box_traits_for<typename BoxTraits::template rebind_value<NewValue>>;

        // if (BoxTraits::has_value(box)) {
            // if constexpr (std::is_void_v<typename BoxTraits::value_type>) {
              // return NewBoxTraits::lift(std::forward<Fn>(fn)());
            // } else {
              // return NewBoxTraits::lift(std::forward<Fn>(fn)(BoxTraits::value(std::forward<Box>(box))));
            // }
        // }

        // if constexpr (requires { BoxTraits::error(std::forward<Box>(box)); }) {
            // return NewBoxTraits::lift(BoxTraits::error(std::forward<Box>(box)));
        // } else {
            // return NewBoxTraits::lift(BoxTraits::error());
        // }

        // return std::forward<Box>(box) | and_then([] (auto ... v) {
           // return NewBoxTraits::lift(std::forward<decltype(v)>(v)...);
        // });

        return std::forward<Box>(box) | and_then([&fn] (auto &&v) {
           return NewBoxTraits::lift(
               fn(std::forward<decltype(v)>(v)));
               // std::forward<decltype(fn)>(fn)(std::forward<decltype(v)>(v)));
        });
    }
};

} // namespace detail::_transform

inline constexpr detail::pipe_for<detail::_transform::op_fn> transform{};

} // namespace beman::monadics

#endif // BEMAN_MONADICS_DETAIL_TRANSFORM_HPP
