// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_TYPE_TRAITS_DECOMPOSE_HPP
#define BEMAN_MONADICS_TYPE_TRAITS_DECOMPOSE_HPP

#include <beman/monadics/concepts/decomposable.hpp>

namespace beman::monadics {

namespace details::_decompose {

template <typename T>
struct type_info;

template <template <typename...> class T, typename... Args>
struct type_info<T<Args...>> {
    // using type = T;
};

template <typename NewValue, template <typename...> class T, typename OldValue, typename... Args>
auto replace_with(T<OldValue, Args...>*) -> T<NewValue, Args...>;

} // namespace details::_decompose

template <decomposable T>
struct decompose {};

} // namespace beman::monadics

#endif // BEMAN_MONADICS_TYPE_TRAITS_DECOMPOSE_HPP
