// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_CONCEPTS_SAME_AS_HPP
#define BEMAN_MONADICS_CONCEPTS_SAME_AS_HPP

#include <concepts>     // std::same_as
#include <type_traits> // std::remove_cvref_t

namespace beman::monadics {

template <typename T, typename U>
concept same_as = std::same_as<
  std::remove_cvref_t<T>, std::remove_cvref_t<U>
>;

} // namespace beman::monadics

#endif // BEMAN_MONADICS_CONCEPTS_SAME_AS_HPP
