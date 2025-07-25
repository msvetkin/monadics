// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_CONCEPTS_DECOMPOSABLE_HPP
#define BEMAN_MONADICS_CONCEPTS_DECOMPOSABLE_HPP

#include <type_traits> // std::remove_cvref_t

namespace beman::monadics {

template <typename T>
concept decomposable = requires {
    []<template <typename...> class U, typename... Args>(U<Args...>*) {
    }(static_cast<std::remove_cvref_t<T>*>(nullptr));
};

} // namespace beman::monadics

#endif // BEMAN_MONADICS_CONCEPTS_DECOMPOSABLE_HPP
