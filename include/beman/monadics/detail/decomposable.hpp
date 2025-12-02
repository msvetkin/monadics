// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_DECOMPOSABLE_HPP
#define BEMAN_MONADICS_DETAIL_DECOMPOSABLE_HPP

#include <beman/monadics/detail/as_pointer.hpp>

#include <cstddef>

namespace beman::monadics::detail {

template <typename T, std::size_t N>
concept decomposable = requires {
    []<template <typename...> class U, typename... Args>
        requires(sizeof...(Args) >= N)
    (U<Args...>*) {}(as_pointer<T>);
};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_DECOMPOSABLE_HPP
