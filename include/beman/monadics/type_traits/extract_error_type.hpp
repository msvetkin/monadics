// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_TYPE_TRAITS_EXTRACT_ERROR_TYPE_HPP
#define BEMAN_MONADICS_TYPE_TRAITS_EXTRACT_ERROR_TYPE_HPP

#include <type_traits> // std::remove_cvref_t

namespace beman::monadics {

template<typename T>
concept has_error_type = requires {
  typename T::error_type;
};

template<typename T>
struct extract_error_type;

template<has_error_type T>
struct extract_error_type<T> {
  using type = typename T::error_type;
};

template<template<typename ...> typename T,
         typename V,
         typename E,
         typename ... Args
        >
  requires (!has_error_type<T<V, E, Args...>>)
struct extract_error_type<T<V, E, Args...>> {
  using type = E;
};

template<typename Box>
using extract_error_type_t = typename extract_error_type<std::remove_cvref_t<Box>>::type;

} // namespace beman::monadics

#endif // BEMAN_MONADICS_TYPE_TRAITS_EXTRACT_ERROR_TYPE_HPP
