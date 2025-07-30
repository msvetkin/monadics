// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_OR_ELSE
#define BEMAN_MONADICS_OR_ELSE

#include <beman/monadics/concepts/same_template_as.hpp>
#include <beman/monadics/type_traits/invoke_result.hpp>
#include "beman/monadics/type_traits/rebind_value.hpp"
#include <beman/monadics/box_traits.hpp>
#include <beman/monadics/pipe.hpp>

#include <type_traits> // std::decay_t
#include <utility>     // std::forward

namespace beman::monadics {

namespace details::_or_else {

template<typename Box, typename Fn, typename Traits>
concept invocable = requires {
    requires Traits::support_error;
    requires std::invocable<
      Fn,
      decltype(Traits::error(std::declval<Box>()))
    >;
} || requires {
    requires !Traits::support_error;
    requires std::invocable<Fn>;
};

template<typename Box, typename Fn, typename Traits>
using invoke_result_t = decltype([] {
    if constexpr (Traits::support_error) {
        return std::type_identity<std::invoke_result_t<Fn, decltype(Traits::error(std::declval<Box>()))>>{};
    } else {
        return std::type_identity<std::invoke_result_t<Fn>>{};
    }
}())::type;

template<typename Box, typename Fn, typename Traits>
  requires invocable<Box, Fn, Traits>
using return_t = rebind_value<Box, invoke_result_t<Box, Fn, Traits>>;

struct op_fn {
  template<typename Box, typename Fn, typename Traits = box_traits_for<Box>>
  [[nodiscard]] inline constexpr auto operator()(Box&& box, Fn&& fn) const noexcept
    -> return_t<decltype(box), decltype(fn), Traits>
  {
      if (Traits::has_value(box)) {
          return Traits::value(std::forward<Box>(box));
      }

      if constexpr (Traits::support_error) {
          return std::invoke(std::forward<Fn>(fn), Traits::error(std::forward<Box>(box)));
      } else {
          return std::invoke(std::forward<Fn>(fn));
      }
  }
};


// template <typename Box, typename F, typename R = std::invoke_result_t<F> >
    // requires requires { requires same_template_as<Box, R>; }
// using or_else_return = rebind_value<Box, R>;

// struct fn {
    // template <typename Fn>
    // struct action {
        // template <typename Box, typename A>
            // requires(std::same_as<std::remove_cvref_t<A>, action>)
        // [[nodiscard]] friend inline constexpr auto
        // operator|(Box&& box, A&& a) noexcept -> or_else_return<decltype(box), decltype(a.fn)> {
            // using Traits = box_traits_for<Box>;

            // if (Traits::has_value(box)) {
                // return Traits::value(std::forward<Box>(box));
            // }

            // if constexpr (Traits::support_error) {
                // return std::invoke(std::forward<Fn>(a.fn), Traits::error(std::forward<Box>(box)));
            // } else {
                // return std::invoke(std::forward<Fn>(a.fn));
            // }
        // }

        // Fn fn;
    // };

    // template <typename Fn>
    // [[nodiscard]] constexpr inline decltype(auto) operator()(Fn&& fn) const noexcept {
        // return action<decltype(std::forward<Fn>(fn))>{std::forward<Fn>(fn)};
    // }
// };

} // namespace details::_or_else

inline constexpr pipe_for<details::_or_else::op_fn{}> or_else{};

} // namespace beman::monadics

#endif // BEMAN_MONADICS_OR_ELSE
