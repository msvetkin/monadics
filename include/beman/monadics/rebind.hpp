// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_REBIND_HPP
#define BEMAN_MONADICS_REBIND_HPP

namespace beman::monadics {

namespace details::_rebind {

template<typename T>
concept HasValue = requires {
  typename T::value_type;
} || requires {
  typename T::element_type;
};

template <typename New, template <typename...> class Box, typename Old, typename ... Args>
auto helper(Box<Old, Args...>*) -> Box<New, Args...>;

} // namespace details::_rebind

template<details::_rebind::HasValue T, typename U>
using rebind = decltype(details::_rebind::helper<U>(static_cast<T*>(nullptr)));

} // namespace beman::monadics

#endif // BEMAN_MONADICS_REBIND_HPP
