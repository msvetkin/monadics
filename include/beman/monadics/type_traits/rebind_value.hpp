// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_TYPE_TRAITS_REBIND_VALUE_HPP
#define BEMAN_MONADICS_TYPE_TRAITS_REBIND_VALUE_HPP

#include <beman/monadics/concepts/decomposable.hpp>

namespace beman::monadics {

namespace details::_rebind_value {

template <typename NewValue, template <typename...> class T, typename OldValue, typename... Args>
auto replace_with(T<OldValue, Args...>*) -> T<NewValue, Args...>;

} // namespace details::_rebind_value

template <decomposable T, typename U>
using rebind_value = decltype(details::_rebind_value::replace_with<U>(static_cast<T*>(nullptr)));

} // namespace beman::monadics

#endif // BEMAN_MONADICS_TYPE_TRAITS_REBIND_VALUE_HPP
