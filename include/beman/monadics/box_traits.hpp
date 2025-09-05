// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_BOX_TRAITS_HPP
#define BEMAN_MONADICS_BOX_TRAITS_HPP

#include <type_traits>

namespace beman::monadics {

template <typename T>
struct box_traits;

namespace detail::_box_traits {

template <typename T>
concept has_specialization = requires {
    typename box_traits<T>;
    { box_traits<T>{} };
};

template <typename T>
struct impl : box_traits<T> {};

} // namespace detail::_box_traits

template <typename T>
concept has_box_traits = requires { requires detail::_box_traits::has_specialization<T>; };

template <has_box_traits T>
using box_traits_for = detail::_box_traits::impl<std::remove_cvref_t<T>>;

} // namespace beman::monadics

#endif // BEMAN_MONADICS_BOX_TRAITS_HPP
