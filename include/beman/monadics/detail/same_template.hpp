
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_SAME_TEMPLATE_HPP
#define BEMAN_MONADICS_DETAIL_SAME_TEMPLATE_HPP

#include <beman/monadics/detail/as_pointer.hpp>
#include <beman/monadics/detail/decomposable.hpp>

namespace beman::monadics::detail {

template <typename T, typename U>
concept same_template = requires {
    requires decomposable<T, 1>;
    requires decomposable<U, 1>;
    []<template <typename...> class C, typename... Ts, typename... Us>(C<Ts...>*, C<Us...>*) {}(as_pointer<T>,
                                                                                                as_pointer<U>);
};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_SAME_TEMPLATE_HPP
