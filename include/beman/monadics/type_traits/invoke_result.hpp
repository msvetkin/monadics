// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_TYPE_TRAITS_INVOKE_RESULT_HPP
#define BEMAN_MONADICS_TYPE_TRAITS_INVOKE_RESULT_HPP

#include <concepts>    // std::invocable
#include <type_traits> // std::invoke_result_t

namespace beman::monadics {

template <typename F, typename V>
    requires(std::invocable<F, V>)
using invoke_result_t = typename std::invoke_result_t<F, V>;

} // namespace beman::monadics

#endif // BEMAN_MONADICS_TYPE_TRAITS_INVOKE_RESULT_HPP
