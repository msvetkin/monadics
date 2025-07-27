// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_CONCEPTS_SAME_TEMPLATE_AS_HPP
#define BEMAN_MONADICS_CONCEPTS_SAME_TEMPLATE_AS_HPP

#include <beman/monadics/concepts/decomposable.hpp>
#include <type_traits>

namespace beman::monadics {

namespace details::_same_template_as {

template <typename T, typename U>
inline constexpr bool compare_impl = false;

template <template <typename...> class C, typename... Ts, typename... Us>
inline constexpr bool compare_impl<C<Ts...>, C<Us...>> = true;

template <typename T, typename U>
concept compare = compare_impl<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;

} // namespace details::_same_template_as

template <typename T, typename U>
concept same_template_as = requires {
    requires decomposable<T>;
    requires decomposable<U>;
    requires details::_same_template_as::compare<T, U>;
    // []<template<typename...> class D, typename... Ts, typename... Us>(D<Ts...>*, D<Us...>) {}
    // (static_cast<std::remove_cvref_t<T>*>(nullptr), static_cast<std::remove_cvref_t<U>*>(nullptr));
};

} // namespace beman::monadics

#endif // BEMAN_MONADICS_CONCEPTS_SAME_TEMPLATE_AS_HPP
