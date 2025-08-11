// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_CONCEPTS_INSTANCE_OF_HPP
#define BEMAN_MONADICS_CONCEPTS_INSTANCE_OF_HPP

#include <beman/monadics/concepts/decomposable.hpp>

#include <concepts> // std::same_as

namespace beman::monadics {

namespace details::_instance_of {

template <typename T, template <typename...> class U>
inline constexpr bool same_as = false;

template <template <typename...> class T, typename... Args, template <typename...> class U>
inline constexpr bool same_as<T<Args...>, U> = requires { requires std::same_as<T<Args...>, U<Args...>>; };

} // namespace details::_instance_of

template <typename T, template <typename...> class U>
concept instance_of = requires {
    requires decomposable<T>;
    requires details::_instance_of::same_as<std::remove_cvref_t<T>, U>;
};

} // namespace beman::monadics

#endif // BEMAN_MONADICS_CONCEPTS_INSTANCE_OF_HPP
