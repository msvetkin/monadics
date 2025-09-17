// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_EXTRACT_VALUE_TYPE_HPP
#define BEMAN_MONADICS_DETAIL_EXTRACT_VALUE_TYPE_HPP

#include <type_traits>

namespace beman::monadics::detail {

template <typename T>
concept has_value_type = requires { typename T::value_type; };

template <typename T>
struct extract_value_type;

template <has_value_type T>
struct extract_value_type<T> {
    using type = typename T::value_type;
};

template <template <typename...> typename T, typename V, typename... Args>
    requires(!has_value_type<T<V, Args...>>)
struct extract_value_type<T<V, Args...>> {
    using type = V;
};

template <typename Box>
using extract_value_type_t = typename extract_value_type<std::remove_cvref_t<Box>>::type;

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_EXTRACT_VALUE_TYPE_HPP
