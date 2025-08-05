// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_TRANSFORM_HPP
#define BEMAN_MONADICS_TRANSFORM_HPP

#include <beman/monadics/concepts/same_template_as.hpp>
#include <beman/monadics/type_traits/invoke_result.hpp>
#include "beman/monadics/type_traits/rebind_value.hpp"
#include <beman/monadics/box_traits.hpp>
#include <beman/monadics/pipe.hpp>

#include <type_traits> // std::decay_t
#include <utility>     // std::forward

// namespace beman::monadics {

// namespace details::_transform {

// template <typename Box, typename F, typename R = box_traits_for<Box>::template invoke_value<Box, F>>
    // requires requires {
        // requires !same_template_as<Box, R>;
        // typename rebind_value<Box, R>;
    // }
// using transform_return = rebind_value<Box, R>;

// struct fn {
    // template <typename Fn>
    // struct action {
        // template <typename Box, typename A>
            // requires(std::same_as<std::remove_cvref_t<A>, action>)
        // [[nodiscard]] friend inline constexpr auto operator|(Box&& box, A&& a) noexcept -> transform_return<decltype(box), decltype(a.fn)> {
            // using Traits = box_traits_for<Box>;

            // if (!Traits::has_value(box)) {
                // if constexpr (Traits::support_error) {
                    // return Traits::error(std::forward<Box>(box));
                // } else {
                    // return Traits::error();
                // }
            // }

            // return std::invoke(std::forward<Fn>(a.fn), Traits::value(std::forward<Box>(box)));
        // }

        // Fn fn;
    // };

    // template <typename Fn>
    // [[nodiscard]] constexpr inline decltype(auto) operator()(Fn&& fn) const noexcept {
        // return action<decltype(std::forward<Fn>(fn))>{std::forward<Fn>(fn)};
    // }
// };

// } // namespace details::_transform

// inline constexpr details::_transform::fn transform{};

// } // namespace beman::monadics

namespace beman::monadics {

namespace details::_transform {

template <typename Box, typename F, typename Traits,
          typename R = invoke_result_t<F, decltype(Traits::value(std::declval<Box>()))>>
    requires requires {
        requires !same_template_as<Box, R>;
        typename Traits::template rebind_value<R>;
        // requires std::same_as<
        // typename std::remove_cvref_t<T>::error_type,
        // typename std::remove_cvref_t<R>::error_type>;
    }
using transform_return = typename Traits::template rebind_value<R>;

struct op_fn {
  template<typename Box, typename Fn, typename Traits = box_traits_for<Box>>
    requires requires {
      typename transform_return<Box, Fn, Traits>;
    }
  [[nodiscard]] inline constexpr auto operator()(Box&& box, Fn&& fn) const noexcept -> transform_return<decltype(box), decltype(fn), box_traits_for<Box>> {
      // return box | and_then([] (auto v) -> Traits::rebind_value<transform_return> { return fn(v); })

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

} // namespace details::_transform

// inline constexpr details::_transform::fn transform{};

inline constexpr pipe_for<details::_transform::op_fn{}> transform{};

} // namespace beman::monadics

#endif // BEMAN_MONADICS_TRANSFORM_HPP
