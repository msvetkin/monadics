// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_BOX_TRAITS_HPP
#define BEMAN_MONADICS_BOX_TRAITS_HPP

#include <concepts>
#include <type_traits>
#include <utility>
#include <functional>

#include <beman/monadics/detail/as_pointer.hpp>
#include <beman/monadics/detail/decomposable.hpp>
#include <beman/monadics/detail/meta_extract_value_type.hpp>
#include <beman/monadics/detail/meta_rebind_value.hpp>
#include <beman/monadics/detail/meta_rebind_error.hpp>
#include <beman/monadics/detail/instance_of.hpp>
#include <beman/monadics/detail/same_template.hpp>
#include <beman/monadics/detail/same_unqualified_as.hpp>

namespace beman::monadics {

/*
template <typename T>
struct box_traits {
  using box_type = T;
};

namespace detail::_box_traits {

template <typename T>
concept False = false;

template <typename T>
struct ExtractValueType;

template <template <typename...> typename T, typename V, typename... Args>
struct ExtractValueType<T<V, Args...>> {
    using type = V;
};

template <typename T>
concept decomposable = requires {
    []<template <typename...> class U, typename... Args>(U<Args...>*) {
    }(static_cast<std::remove_cvref_t<T>*>(nullptr));
};

template <typename Traits, typename Box>
    requires requires {
      typename Traits::value_type;
    } || requires {
      typename Box::value_type;
    } || requires {
      requires decomposable<Box>;
      typename ExtractValueType<Box>;
    }
consteval auto valueType() noexcept {
    if constexpr (requires { typename Traits::value_type; }) {
        return std::type_identity<typename Traits::value_type>{};
    } else if constexpr (requires { typename Box::value_type; }) {
        return std::type_identity<typename Box::value_type>{};
    } else if constexpr (requires { typename ExtractValueType<Box>; }) {
        return std::type_identity<typename ExtractValueType<Box>::type>{};
    } else {
        // static_assert(False<Box>, "Not able to extract value_type");
    }
}

template <typename Traits, typename Box>
using ValueType = decltype(valueType<Traits, Box>())::type;

template <typename Traits, typename Box>
consteval auto errorType() noexcept {
    if constexpr (requires { typename Traits::error_type; }) {
        return std::type_identity<typename Traits::error_type>{};
    } else if constexpr (requires { typename Box::error_type; }) {
        return std::type_identity<typename Box::error_type>{};
    } else if constexpr (requires { Traits::error(); }) {
        return std::type_identity<std::remove_cvref_t<decltype(Traits::error())>>{};
        // } else if constexpr (requires { typename ExtractErrorType<Box>; }) {
        // return std::type_identity<typename ExtractErrorType<Box>>{};
    } else {
        static_assert(False<Box>, "Not able to extract error_type");
    }
}

template <typename Traits, typename Box>
using ErrorType = decltype(errorType<Traits, Box>())::type;

template <typename V, template <typename...> class Box, typename T, typename... Args>
auto metaRebind(Box<T, Args...>*) -> Box<V, Args...>;

template <typename Traits, typename Box, typename T>
consteval auto rebindValue() noexcept {
    if constexpr (requires { typename Traits::template rebind<T>; }) {
        return std::type_identity<typename Traits::template rebind<T>>{};
    } else if constexpr (requires { typename Box::template rebind<T>; }) {
        return std::type_identity<typename Box::template rebind<T>>{};
    } else if constexpr (requires { metaRebind<T>(static_cast<Box*>(nullptr)); }) {
        return std::type_identity<decltype(metaRebind<T>(static_cast<Box*>(nullptr)))>{};
    } else {
        static_assert(False<Box>, "Not able to find rebind");
    }
}

template <typename Traits, typename Box, typename T>
using Rebind = decltype(rebindValue<Traits, Box, T>())::type;

template <typename Traits, typename Box, typename E>
consteval auto rebindError() noexcept {
    if constexpr (requires { typename Traits::template rebind_error<E>; }) {
        return std::type_identity<typename Traits::template rebind_error<E>>{};
    } else if constexpr (requires { typename Box::template rebind_error<E>; }) {
        return std::type_identity<typename Box::template rebind_error<E>>{};
    } else if constexpr (requires { Traits::error(); }) {
        return std::type_identity<Box>{};
    } else if constexpr (requires { metaRebindError<E>(static_cast<Box*>(nullptr)); }) {
        return std::type_identity<decltype(metaRebindError<E>(static_cast<Box*>(nullptr)))>{};
    } else {
        static_assert(False<Box>, "Not able to find rebind_error");
    }
}

template <typename Traits, typename Box, typename E>
using RebindError = decltype(rebindError<Traits, Box, E>())::type;

template <typename Traits, typename Box, typename Value>
consteval auto liftValue() noexcept {
    if constexpr (requires { Traits::lift(); }) {
        return &Traits::lift;
    } else if constexpr (requires { Traits::lift(Traits::value(std::declval<Box>())); }) {
        return [](auto&& v) -> decltype(Traits::lift(std::forward<decltype(v)>(v))) {
            return Traits::lift(std::forward<decltype(v)>(v));
        };
        // return [](auto&& v) { return Traits::lift(std::forward<decltype(v)>(v)); };
    } else if constexpr (std::is_void_v<Value>) {
        return []() -> Box { return {}; };
    } else {
        return [](auto&& v) -> Box { return std::forward<decltype(v)>(v); };
    }
};

template <typename Traits, typename Box>
consteval auto liftError() noexcept {
    if constexpr (requires { Traits::lift_error(); }) {
        return &Traits::lift_error;
    } else if constexpr (requires { Traits::lift_error(Traits::error()); }) {
        return [](auto&& e) { return Traits::lift_error(std::forward<decltype(e)>(e)); };
    } else if constexpr (requires { Traits::error(); }) {
        return []() -> Box { return {}; };
    } else {
        return [](auto&& e) -> Box { return std::forward<decltype(e)>(e); };
    }
};

template <typename Traits, typename Box>
consteval auto getValue() noexcept {
    if constexpr (requires { std::declval<Box>().value(); }) {
        return [](auto&& b) { return std::forward<decltype(b)>(b).value(); };
    } else {
        return [](auto&& b) -> decltype(Traits::value(std::forward<decltype(b)>(b))) {
            return Traits::value(std::forward<decltype(b)>(b));
        };
    }
};

template <typename Traits, typename Box>
consteval auto getError() noexcept {
    if constexpr (requires { std::declval<Box>().error(); }) {
        return [](auto&& b) { return std::forward<decltype(b)>(b).error(); };
        // } else if constexpr (requires { *std::declval<Box>(); }) {
        // return [] (auto &&b) {
        // return *std::forward<decltype(b)>(b);
        // };
    } else if constexpr (requires { std::declval<Traits>().error(); }) {
        return &Traits::error;
    } else {
        return [](auto&& b) { return Traits::error(std::forward<decltype(b)>(b)); };
    }
};

template <typename Traits, typename Box>
consteval auto hasValue() noexcept {
    if constexpr (requires { std::declval<Box>().has_value(); }) {
        return [](const Box& b) { return b.has_value(); };
    } else {
        return &Traits::has_value;
    }
};

template <typename Box, typename Traits = box_traits<Box>>
struct TraitsFor {
    using box_type   = Box;
    using value_type = ValueType<Traits, Box>;
    using error_type = ErrorType<Traits, Box>;

    template <typename T>
    using rebind = Rebind<Traits, Box, T>;

    template <typename E>
    using rebind_error = RebindError<Traits, Box, E>;

    inline static constexpr auto value     = getValue<Traits, Box>();
    inline static constexpr auto error     = getError<Traits, Box>();
    inline static constexpr auto has_value = hasValue<Traits, Box>();

    inline static constexpr auto lift       = liftValue<Traits, Box, value_type>();
    inline static constexpr auto lift_error = liftError<Traits, Box>();
};

template <typename T>
concept has_specialization = requires {
    typename box_traits<T>;
    { box_traits<T>{} };
} || requires {
    typename TraitsFor<std::remove_cvref_t<T>>;
    { TraitsFor<std::remove_cvref_t<T>>{} };
};

template <typename T, typename Traits>
concept has_error_fn = requires {
    { Traits::error(std::declval<T>()) } noexcept -> std::convertible_to<typename Traits::error_type>;
} || requires {
    { Traits::error() } noexcept -> std::convertible_to<typename Traits::error_type>;
};

template <typename T>
concept has_minimal_traits = requires {
    requires has_specialization<T>;
    // { box_traits<T>::has_value(std::declval<T>()) } noexcept -> std::same_as<bool>;
    { box_traits<T>::value(std::declval<T>()) } noexcept -> std::convertible_to<typename box_traits<T>::value_type>;
    // { std::invoke(box_traits<T>::value(std::declval<T>()) } noexcept -> std::convertible_to<typename
    // box_traits<T>::value_type>; { box_traits<T>::value(std::declval<T>()) } noexcept -> std::convertible_to<typename
    // box_traits<T>::value_type>;
    requires has_error_fn<T, box_traits<T>>;
};

template <typename T, typename Traits = box_traits<T>>
concept valid_specialization = requires {
    requires has_minimal_traits<T>;
    typename Traits::value_type;
    typename Traits::template rebind<typename Traits::value_type>;
    typename Traits::error_type;
    typename Traits::template rebind_error<typename Traits::error_type>;
};

template <typename Traits>
concept valid_specialization2 = requires {
    // requires has_minimal_traits<T>;
    typename Traits::value_type;
    typename Traits::template rebind<typename Traits::value_type>;
    typename Traits::error_type;
    typename Traits::template rebind_error<typename Traits::error_type>;

    { Traits::value(std::declval<typename Traits::box_type>()) };
    requires requires {
        { Traits::error() };
    } || requires {
        { Traits::error(std::declval<typename Traits::box_type>()) };
    };
};

} // namespace detail::_box_traits

template <typename T>
concept has_box_traits = requires {
    requires detail::_box_traits::has_specialization<std::remove_cvref_t<T>>;
    requires detail::_box_traits::valid_specialization<std::remove_cvref_t<T>>;
};

template <typename T>
concept has_box_traits2 = requires {
    typename box_traits<T>;
    { box_traits<T>{} };
    typename detail::_box_traits::TraitsFor<std::remove_cvref_t<T>>;
    { detail::_box_traits::TraitsFor<std::remove_cvref_t<T>>{} };
    requires detail::_box_traits::valid_specialization2<detail::_box_traits::TraitsFor<std::remove_cvref_t<T>>>;
};

#if !defined(BEMAN_MONADICS_BOX_TRAITS_BUILDER)
template <has_box_traits T>
using box_traits_for = box_traits<std::remove_cvref_t<T>>;
#else
template <typename T>
using box_traits_for = detail::_box_traits::TraitsFor<std::remove_cvref_t<T>>;
#endif
*/

/*

has_specialization<T>
  box_traits<T>
  has_minimal_box_traits<T>
    box_traits<T>::has_value();
    box_traits<T>::value();
    box_traits<T>::error();
  has_full_box_traits<T>
    box_traits<T>::value_type
    box_traits<T>::error_type
    box_traits<T>::rebind
    box_traits<T>::rebind_error
    box_traits<T>::has_value();
    box_traits<T>::value();
    box_traits<T>::error();

  box_traits<T>;
  box_traits<T>::value_type   || deducable_value_type<T>;
  box_traits<T>::error_type   || deducable_error_type<T>;
  box_traits<T>::rebind || deducable_rebind<T>;
  box_traits<T>::rebind_error || deducable_rebind_error<T>;

template<typename T>
struct box_traits_for;

template<typename T>
  requires requires {
    has_specialization<T>;
    has_minimal_traits<T>;
    has_full_traits<T>;
  }
struct box_traits_for {
  using traits_type = box_traits<T>;
};

template<has_minimal_traits T>
  requires requires {
    has_specialization
    has_minimal_traits
    has_deducable_traits
  }
struct box_traits_for {
 private:
  using base_traits = box_traits<T>;

 public:
   struct traits_type : base_traits {
      using value_type = get_value_type_or_deduce<T, base_traits>
      using error_type = get_error_type_or_deduce<T, base_traits>

      template<typename V>
      using rebind = get_rebind_error_or_deduce<V, base_traits>

      template<typename E>
      using error_type = get_rebind_error_or_deduce<E, base_traits>
   };
};


*/

namespace detail::_box_traits {

template <typename Box, typename Trait>
[[nodiscard]] consteval auto deduce_value_type() noexcept {
    if constexpr (requires { typename Trait::value_type; }) {
        return std::type_identity<typename Trait::value_type>{};
    } else if constexpr (requires { typename Box::value_type; }) {
        return std::type_identity<typename Box::value_type>{};
    } else if constexpr (requires { meta_extract_value_type<Box>(); }) {
        return meta_extract_value_type<Box>();
    }
}

template <typename Box, typename Traits>
concept has_value_type = requires {
    { deduce_value_type<Box, Traits>() } -> instance_of<std::type_identity>;
};

template <typename Box, typename Traits>
    requires has_value_type<Box, Traits>
using value_type_t = typename decltype(deduce_value_type<Box, Traits>())::type;

template <typename Box, typename Trait>
consteval auto deduce_error_type() noexcept {
    if constexpr (requires { typename Trait::error_type; }) {
        return std::type_identity<typename Trait::error_type>{};
    } else if constexpr (requires { typename Box::error_type; }) {
        return std::type_identity<typename Box::error_type>{};
    } else if constexpr (requires { Trait::error(); }) {
        return std::type_identity<std::remove_cvref_t<decltype(Trait::error())>>{};
    }
}

template <typename Box, typename Traits>
concept has_error_type = requires {
    { deduce_error_type<Box, Traits>() } -> instance_of<std::type_identity>;
};

template <typename Box, typename Traits>
    requires has_error_type<Box, Traits>
using error_type_t = typename decltype(deduce_error_type<Box, Traits>())::type;

template <typename Box, typename Traits, typename T>
consteval auto deduce_rebind() noexcept {
    if constexpr (requires { typename Traits::template rebind<T>; }) {
        return std::type_identity<Traits>{};
    } else if constexpr (requires { typename Box::template rebind<T>; }) {
        return std::type_identity<Box>{};
    } else if constexpr (requires { get_meta_rebind<Box>(); }) {
        return get_meta_rebind<Box>();
    }
}

template <typename Box, typename Traits, typename T>
concept has_rebind = requires {
    { deduce_rebind<Box, Traits, T>() } -> instance_of<std::type_identity>;
};

template <typename Box, typename Traits, typename T>
    requires has_rebind<Box, Traits, T>
using rebind = typename decltype(deduce_rebind<Box, Traits, T>())::type;

template <typename Box>
struct no_rebind_error {
    template <typename>
    using rebind_error = Box;
};

template <typename Box, typename Traits, typename E>
consteval auto deduce_rebind_error() noexcept {
    if constexpr (requires { typename Traits::template rebind_error<E>; }) {
        return std::type_identity<Traits>{};
    } else if constexpr (requires { typename Box::template rebind_error<E>; }) {
        return std::type_identity<Box>{};
    } else if constexpr (requires { Traits::error(); }) {
        return std::type_identity<no_rebind_error<Box>>{};
    } else if constexpr (requires { get_meta_rebind_error<Box>(); }) {
        return get_meta_rebind_error<Box>();
    } else {
        // static_assert(False<Box>, "I was not able to identify rebind alias, pleases read the docs");
    }
}

template <typename Box, typename Traits, typename E>
concept has_rebind_error = requires {
    { deduce_rebind_error<Box, Traits, E>() } -> instance_of<std::type_identity>;
};

template <typename Box, typename Traits, typename E>
    requires has_rebind_error<Box, Traits, E>
using rebind_error = typename decltype(deduce_rebind_error<Box, Traits, E>())::type;

template <typename Fn, typename Box, typename R>
concept invocable_r = requires {
    requires std::invocable<Fn, Box>;
    { std::invoke(std::declval<Fn>(), std::declval<Box>()) } -> same_unqualified_as<R>;
};

template <typename Box, typename Trait>
consteval auto deduce_value_query_fn() noexcept {
    if constexpr (requires { Trait::has_value(std::declval<Box>()); }) {
        return [](const auto& box) { return Trait::has_value(box); };
    } else if constexpr (requires { std::declval<Box>().has_value(); }) {
        return [](const auto& box) { return box.has_value(); };
    }
};

template <typename Box, typename Traits>
concept has_value_query_fn = requires {
    { deduce_value_query_fn<Box, Traits>() } -> std::invocable<Box>;
    { deduce_value_query_fn<Box, Traits>()(std::declval<Box>()) } -> std::same_as<bool>;
};

template <typename Box, typename Traits>
    requires has_value_query_fn<Box, Traits>
inline constexpr auto value_query_fn = deduce_value_query_fn<Box, Traits>();

template <typename Box, typename Traits>
consteval auto deduce_value_fn() noexcept {
    if constexpr (requires { Traits::value(std::declval<Box>()); }) {
        return [](auto&& b) -> decltype(Traits::value(std::forward<decltype(b)>(b))) {
            return Traits::value(std::forward<decltype(b)>(b));
        };
    } else if constexpr (requires { std::declval<Box>().value(); }) {
        return [](auto&& b) { return std::forward<decltype(b)>(b).value(); };
    }
};

template <typename Box, typename Traits>
concept has_value_fn = requires {
    { deduce_value_fn<Box, Traits>() } -> std::invocable<Box>;
    requires has_value_type<Box, Traits>;
    { deduce_value_fn<Box, Traits>()(std::declval<Box>()) } -> std::convertible_to<value_type_t<Box, Traits>>;
};

template <typename Box, typename Traits>
    requires has_value_fn<Box, Traits>
inline constexpr auto value_fn = deduce_value_fn<Box, Traits>();

template <typename Box, typename Traits>
consteval auto deduce_error_fn() noexcept {
    if constexpr (requires { Traits::error(); }) {
        return []() { return Traits::error(); };
        // return [](auto&&) { return Traits::error(); };
    } else if constexpr (requires { Traits::error(std::declval<Box>()); }) {
        return [](auto&& b) { return Traits::error(std::forward<decltype(b)>(b)); };
    } else if constexpr (requires { std::declval<Box>().error(); }) {
        return [](auto&& b) { return std::forward<decltype(b)>(b).error(); };
    };
};

template <typename Box, typename Traits>
concept has_error_fn = requires {
    requires has_error_type<Box, Traits>;
    requires requires {
        { deduce_error_fn<Box, Traits>() } -> std::invocable<Box>;
        { deduce_error_fn<Box, Traits>()(std::declval<Box>()) } -> std::convertible_to<error_type_t<Box, Traits>>;
    } || requires {
        { deduce_error_fn<Box, Traits>() } -> std::invocable;
        { deduce_error_fn<Box, Traits>()() } -> std::convertible_to<error_type_t<Box, Traits>>;
    };
};

template <typename Box, typename Traits>
    requires has_error_fn<Box, Traits>
inline constexpr auto error_fn = deduce_error_fn<Box, Traits>();

template <typename Box, typename Traits, typename T>
consteval auto deduce_lift_fn() noexcept {
    if constexpr (requires { Traits::lift(std::declval<T>()); }) {
        return [](auto&& v) { return Traits::lift(std::forward<decltype(v)>(v)); };
    } else if constexpr (requires { &Traits::lift; }) {
        // return &Traits::lift;
        return [](auto& v) { return Traits::lift(std::forward<decltype(v)>(v)); };
    } else if constexpr (std::is_void_v<T>) {
        return []() { return Box{}; };
    } else if constexpr (std::constructible_from<Box, T>) {
        return [](auto&& v) { return Box{std::forward<decltype(v)>(v)}; };
    }
};

template <typename Box, typename Traits, typename T>
concept has_lift_fn = requires {
    requires requires {
        { deduce_lift_fn<Box, Traits, T>() } -> std::invocable<T&>; // T& for raw_ptr value
        // { deduce_lift_fn<Box, Traits, T>()(std::declval<T>()) } -> same_unqualified_as<Box>;
        // { deduce_lift_fn<Box, Traits, T>()(std::declval<T>()) } -> same_unqualified_as<Box>;
    } || requires {
        requires std::is_void_v<T>;
        { deduce_lift_fn<Box, Traits, T>() } -> std::invocable;
        // { deduce_lift_fn<Box, Traits, T>() } -> same_unqualified_as<Box>;
    };
};

template <typename Box, typename Traits, typename T>
    requires has_lift_fn<Box, Traits, T>
inline constexpr auto lift_fn = deduce_lift_fn<Box, Traits, T>();

template <typename Box, typename Traits, typename E>
consteval auto deduce_lift_error_fn() noexcept {
    if constexpr (requires { Traits::lift_error(std::declval<E>()); }) {
        return [](auto&& e) { return Traits::lift_error(std::forward<decltype(e)>(e)); };
    } else if constexpr (std::constructible_from<Box, E>) {
        return [](auto&& e) { return Box{std::forward<decltype(e)>(e)}; };
    }
};

template <typename Box, typename Traits, typename E>
concept has_lift_error_fn = requires {
    requires requires {
        { deduce_lift_error_fn<Box, Traits, E>() } -> std::invocable<E>;
        { deduce_lift_error_fn<Box, Traits, E>()(std::declval<E>()) } -> same_unqualified_as<Box>;
    } || requires {
        { deduce_lift_error_fn<Box, Traits, E>() } -> std::invocable;
        { deduce_lift_error_fn<Box, Traits, E>()() } -> same_unqualified_as<Box>;
    };
};

template <typename Box, typename Traits, typename E>
    requires has_lift_error_fn<Box, Traits, E>
inline constexpr auto lift_error_fn = deduce_lift_error_fn<Box, Traits, E>();

template <typename Box, typename Traits>
concept is_box = requires {
    requires has_value_type<Box, Traits>;
    requires has_error_type<Box, Traits>;
    requires has_rebind<Box, Traits, value_type_t<Box, Traits>>;
    requires has_rebind_error<Box, Traits, error_type_t<Box, Traits>>;

    requires has_value_query_fn<Box, Traits>;
    requires has_value_fn<Box, Traits>;
    requires has_error_fn<Box, Traits>;

    requires has_lift_fn<Box, Traits, value_type_t<Box, Traits>>;
    requires has_lift_error_fn<Box, Traits, error_type_t<Box, Traits>>;
};

template <typename Box, typename Traits>
struct deduce_traits {
    using box_type   = Box;
    using value_type = value_type_t<Box, Traits>;
    using error_type = error_type_t<Box, Traits>;

    template <typename T>
    using rebind = rebind<Box, Traits, value_type>::template rebind<T>;

    template <typename E>
    using rebind_error = rebind_error<Box, Traits, error_type>::template rebind_error<E>;

    inline static constexpr auto has_value = value_query_fn<Box, Traits>;
    inline static constexpr auto value     = value_fn<Box, Traits>;
    inline static constexpr auto error     = error_fn<Box, Traits>;

    inline static constexpr auto lift       = lift_fn<Box, Traits, value_type>;
    inline static constexpr auto lift_error = lift_error_fn<Box, Traits, error_type>;

    template <typename Fn, typename B>
    // requires requires {
    // requires std::is_void_v<value_type>;
    // requires std::invocable<Fn>;
    // } || requires { requires std::invocable<Fn, value_type>; }
    [[nodiscard]] static constexpr decltype(auto) invoke_with_value(Fn&& fn, B&& box) noexcept {
        if constexpr (std::is_void_v<value_type> && std::invocable<Fn>) {
            // should just invoke Traits::value(box);
            return std::forward<Fn>(fn)();
        } else {
            return std::forward<Fn>(fn)(value(std::forward<B>(box)));
        }
    }

    template <typename Fn, typename B>
        requires requires {
            { error(std::declval<Box>()) };
            requires std::invocable<Fn, error_type>;
        } || requires { requires std::invocable<Fn>; }
    [[nodiscard]] static constexpr decltype(auto) invoke_with_error(Fn&& fn, B&& box) noexcept {
        if constexpr (requires { error(std::forward<B>(box)); }) {
            return std::forward<Fn>(fn)(error(std::forward<B>(box)));
        } else {
            return std::forward<Fn>(fn)();
        }
    }

    template <typename B>
    [[nodiscard]] static constexpr decltype(auto) lift_with_value(B&& box) noexcept {
        if constexpr (std::is_void_v<value_type>) {
            return lift();
        } else {
            return lift(value(std::forward<B>(box)));
        }
    }

    template <typename B>
    [[nodiscard]] static constexpr decltype(auto) lift_with_error(B&& box) noexcept {
        if constexpr (requires { error(std::forward<B>(box)); }) {
            return lift_error(error(std::forward<B>(box)));
            // } else if constexpr (requires { error(); }) {
            // return lift_error();
        } else {
            return lift_error(error());
        }
    }
};

} // namespace detail::_box_traits

template <typename T>
struct box_traits {
    using box_type = T;
};

template <typename Box>
concept is_box = requires {
    typename box_traits<std::remove_cvref_t<Box>>;
    requires detail::_box_traits::is_box<std::remove_cvref_t<Box>, // maybe should preserve qualifiers?
                                         box_traits<std::remove_cvref_t<Box>>>;
};

template <is_box T>
using box_traits_for = detail::_box_traits::deduce_traits<std::remove_cvref_t<T>, box_traits<std::remove_cvref_t<T>>>;

} // namespace beman::monadics

#endif // BEMAN_MONADICS_BOX_TRAITS_HPP
