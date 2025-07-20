// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_CONCEPTS_HPP
#define BEMAN_MONADICS_CONCEPTS_HPP

#include <concepts> // std::same_as
#include <type_traits> // std::remove_cvref_t

namespace beman::monadics {

namespace details::_concepts {

template<typename T>
concept Box = requires(T &&t) {
  typename T::value_type;
  { t.has_value() } -> std::same_as<bool>;
  // { t.value() } -> std::same_as<typename T::value_type>;
};

} // namespace details::_concepts

template<typename T>
concept Box = details::_concepts::Box<std::remove_cvref_t<T>>;

} // namespace beman::monadics

#endif // BEMAN_MONADICS_CONCEPTS_HPP
