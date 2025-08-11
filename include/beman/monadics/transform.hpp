// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_TRANSFORM_HPP
#define BEMAN_MONADICS_TRANSFORM_HPP

#include <beman/monadics/concepts/same_template_as.hpp>
#include <beman/monadics/type_traits/invoke_result.hpp>
#include "beman/monadics/type_traits/rebind_value.hpp"
#include <beman/monadics/box_traits.hpp>
#include <beman/monadics/pipe.hpp>
#include <beman/monadics/and_then.hpp>

#include <utility> // std::forward

namespace beman::monadics {

namespace details::_transform {

template <typename Box,
          typename F,
          typename Traits,
          typename R = invoke_result_t<F, decltype(Traits::value(std::declval<Box>()))>>
    requires requires {
        requires !same_template_as<Box, R>;
        typename Traits::template rebind_value<R>;
        // requires std::same_as<
        // typename std::remove_cvref_t<T>::error_type,
        // typename std::remove_cvref_t<R>::error_type>;
    }
using transform_return_t = typename Traits::template rebind_value<R>;

struct op_fn {
    template <typename Box, typename Fn, typename Traits = box_traits_for<Box>>
        requires requires { typename transform_return_t<Box, Fn, Traits>; }
    [[nodiscard]] inline constexpr auto operator()(Box box, Fn fn) const noexcept {
        return std::forward<Box>(box) | and_then([&](auto&& v) -> transform_return_t<Box, Fn, Traits> {
                   // if constexpr (R is void) {}

                   return std::forward<Fn>(fn)(std::forward<decltype(v)>(v));
               });
    }
};

} // namespace details::_transform

inline constexpr pipe_for<details::_transform::op_fn> transform{};

} // namespace beman::monadics

#endif // BEMAN_MONADICS_TRANSFORM_HPP
