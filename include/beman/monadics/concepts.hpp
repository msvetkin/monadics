// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_CONCEPTS_HPP
#define BEMAN_MONADICS_CONCEPTS_HPP

#include <concepts>    // std::same_as
#include <type_traits> // std::remove_cvref_t

namespace beman::monadics {

namespace details::_concepts {

template <typename T>
concept Box = requires(T&& t) {
    typename T::value_type;
    { t.has_value() } -> std::same_as<bool>;
    // { t.value() } -> std::same_as<typename T::value_type>;
};

template <typename T1, template <typename...> typename T2>
struct same_template_as {
    inline static constexpr bool value = false;
};

template <template <typename...> class T1, typename... Args1, template <typename...> class T2>
struct same_template_as<T1<Args1...>, T2> {
    inline static constexpr bool value = requires {
        typename T2<Args1...>;
        requires std::same_as<T1<Args1...>, T2<Args1...>>;
    };
};

} // namespace details::_concepts

// template <typename T1, template <typename...> typename T2>
// concept same_template_as = requires {
// []<typename... Args>(T2<Args...>*) {
// requires std::same_as<std::remove_cvref_t<T1>, T2<Args...>>;
// }(static_cast<std::remove_cvref_t<T1>*>(nullptr));
// };

template <typename T>
concept decomposable = requires {
    []<template <typename...> typename U, typename... Args>(U<Args...>*) {
    }(static_cast<std::remove_cvref_t<T>*>(nullptr));
};

template <typename T, template <typename...> class U>
concept same_template_as = requires {
    requires decomposable<T>;
    []<typename... Args>(U<Args...>*) {}(static_cast<std::remove_cvref_t<T>*>(nullptr));
};

} // namespace beman::monadics

#endif // BEMAN_MONADICS_CONCEPTS_HPP
