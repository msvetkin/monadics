// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_BOX_TRAITS_HPP
#define BEMAN_MONADICS_BOX_TRAITS_HPP

#include <concepts>
#include <type_traits>
#include <utility>

namespace beman::monadics {

template <typename T>
struct box_traits;

namespace detail::_box_traits {

template <typename T>
concept False = false;

template <typename T>
struct ExtractValueType;

template <template <typename...> typename T, typename V, typename... Args>
struct ExtractValueType<T<V, Args...>> {
    using type = V;
};

template <typename Traits, typename Box>
consteval auto valueType() noexcept {
    if constexpr (requires { typename Traits::value_type; }) {
        return std::type_identity<typename Traits::value_type>{};
    } else if constexpr (requires { typename Box::value_type; }) {
        return std::type_identity<typename Box::value_type>{};
    } else if constexpr (requires { typename ExtractValueType<Box>; }) {
        return std::type_identity<typename ExtractValueType<Box>::type>{};
    } else {
        static_assert(False<Box>, "Not able to extract value_type");
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
auto metaRebindValue(Box<T, Args...>*) -> Box<V, Args...>;

template <typename Traits, typename Box, typename T>
consteval auto rebindValue() noexcept {
    if constexpr (requires { typename Traits::template rebind_value<T>; }) {
        return std::type_identity<typename Traits::template rebind_value<T>>{};
    } else if constexpr (requires { typename Box::template rebind<T>; }) {
        return std::type_identity<typename Box::template rebind<T>>{};
    } else if constexpr (requires { metaRebindValue<T>(static_cast<Box*>(nullptr)); }) {
        return std::type_identity<decltype(metaRebindValue<T>(static_cast<Box*>(nullptr)))>{};
    } else {
        static_assert(False<Box>, "Not able to find rebind_value");
    }
}

template <typename Traits, typename Box, typename T>
using RebindValue = decltype(rebindValue<Traits, Box, T>())::type;

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
        return [](auto&& v) { return Traits::lift(std::forward<decltype(v)>(v)); };
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
    using rebind_value = RebindValue<Traits, Box, T>;

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
    typename Traits::template rebind_value<typename Traits::value_type>;
    typename Traits::error_type;
    typename Traits::template rebind_error<typename Traits::error_type>;
};

template <typename Traits>
concept valid_specialization2 = requires {
    // requires has_minimal_traits<T>;
    typename Traits::value_type;
    typename Traits::template rebind_value<typename Traits::value_type>;
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
    box_traits<T>::rebind_value
    box_traits<T>::rebind_error
    box_traits<T>::has_value();
    box_traits<T>::value();
    box_traits<T>::error();

  box_traits<T>;
  box_traits<T>::value_type   || deducable_value_type<T>;
  box_traits<T>::error_type   || deducable_error_type<T>;
  box_traits<T>::rebind_value || deducable_rebind_value<T>;
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
      using rebind_value = get_rebind_error_or_deduce<V, base_traits>

      template<typename E>
      using error_type = get_rebind_error_or_deduce<E, base_traits>
   };
};


*/

} // namespace beman::monadics

#endif // BEMAN_MONADICS_BOX_TRAITS_HPP
