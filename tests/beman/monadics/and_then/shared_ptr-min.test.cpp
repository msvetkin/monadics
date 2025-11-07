// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "beman/monadics/box_traits.hpp"
#include "beman/monadics/detail/and_then.hpp"

#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <memory>
#include <optional>
#include <type_traits>

namespace beman::monadics {

namespace details::_instance_of {

template <typename t, template <typename...> class u>
inline constexpr bool same_as = false;

template <template <typename...> class t, typename... args, template <typename...> class u>
inline constexpr bool same_as<t<args...>, u> = requires { requires std::same_as<t<args...>, u<args...>>; };

} // namespace details::_instance_of

namespace details {

template <typename T>
concept has_value_type = requires { typename T::value_type; };

template <typename T>
struct extract_value_type;

// template<has_value_type T>
// struct extract_value_type<T> {
// using type = typename T::value_type;
// };

template <template <typename...> typename T, typename V, typename... Args>
// requires (!has_value_type<T<V, Args...>>)
struct extract_value_type<T<V, Args...>> {
    using type = V;
};

template <typename Box>
using extract_value_type_t = typename extract_value_type<std::remove_cvref_t<Box>>::type;

template <typename Box>
struct ValueType {
    using value_type = extract_value_type_t<Box>;
};

} // namespace details

template <typename t, template <typename...> class u>
concept instance_of = requires {
    // requires decomposable<t>;
    requires details::_instance_of::same_as<std::remove_cvref_t<t>, u>;
};

template <typename T>
concept decomposable = requires {
    []<template <typename...> class U, typename... Args>(U<Args...>*) {
    }(static_cast<std::remove_cvref_t<T>*>(nullptr));
};

namespace details::_rebind_value {

template <typename NewValue, template <typename...> class Box, typename OldValue, typename... Args>
auto replace_with(Box<OldValue, Args...>*) -> Box<NewValue>;

template <decomposable Box, typename U>
using type = decltype(details::_rebind_value::replace_with<U>(static_cast<Box*>(nullptr)));

} // namespace details::_rebind_value

// template <typename Box, typename T>
// using rebind_value = details::_rebind_value::type<std::remove_cvref_t<Box>, std::remove_cvref_t<T> >;

namespace details {

template <typename Box, typename V>
using RebindValue = details::_rebind_value::type<std::remove_cvref_t<Box>, std::remove_cvref_t<V>>;

} // namespace details

namespace details::_error_type {} // namespace details::_error_type

namespace details {

template <typename BoxTraits>
struct ErrorType;

template <typename BoxTraits>
    requires requires {
        { BoxTraits::error() };
    }
struct ErrorType<BoxTraits> {
    using error_type = std::remove_cvref_t<decltype(BoxTraits::error())>;
};

} // namespace details

namespace details {

// template<typename BoxTraits>
// struct ErrorType;

// template<typename>
// struct RebindError<BoxTraits> {
// using error_type = std::remove_cvref_t<decltype(BoxTraits::error())>;
// };

} // namespace details

template <typename Box>
struct BaseBoxTraits;

template <instance_of<std::shared_ptr> Box>
struct BaseBoxTraits<Box> {
    [[nodiscard]] inline static constexpr bool has_value(const Box& box) noexcept { return static_cast<bool>(box); }

    [[nodiscard]] inline static constexpr decltype(auto) value(auto&& box) noexcept {
        return *std::forward<decltype(box)>(box);
    }

    [[nodiscard]] inline static constexpr auto error() noexcept { return nullptr; }

    [[nodiscard]] inline static constexpr Box lift(auto&& value) noexcept {
        return std::make_shared<int>(std::forward<decltype(value)>(value));
    }
};

namespace details {

namespace details::_rebind_error {

template <typename Box>
struct SameBox {
    template <typename>
    using rebind_error = Box;
};

} // namespace details::_rebind_error

template <typename BoxTraits, typename Box, typename E>
consteval decltype(auto) rebindError() noexcept {
    if constexpr (requires { BoxTraits::error(); }) {
        return std::type_identity<details::_rebind_error::SameBox<Box>>{};
    }
}

// template<typename BoxTraits, typename Box, typename E>
// using RebindError = rebindError
// template<typename BoxTraits, typename Box, typename V>
// using RebindError {

// };

} // namespace details

namespace details::_box_traits_builder {

template <typename T>
concept False = false;

template <typename T>
struct ExtractValueType;

template <template <typename...> typename T, typename V, typename... Args>
struct ExtractValueType<T<V, Args...>> {
    using type = V;
};

template <typename Traits, typename Box>
consteval auto getValueType() noexcept {
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
using ValueType = decltype(getValueType<Traits, Box>())::type;

template <typename Traits, typename Box>
consteval auto getErrorType() noexcept {
    if constexpr (requires { typename Traits::error_type; }) {
        return std::type_identity<typename Traits::error_type>{};
    } else if constexpr (requires { Traits::error(); }) {
        return std::type_identity<std::remove_cvref_t<decltype(Traits::error())>>{};
    } else if constexpr (requires { typename Box::error_type; }) {
        return std::type_identity<typename Box::error_type>{};
        // } else if constexpr (requires { typename ExtractErrorType<Box>; }) {
        // return std::type_identity<typename ExtractErrorType<Box>>{};
    } else {
        static_assert(False<Box>, "Not able to extract error_type");
    }
}

template <typename Traits, typename Box>
using ErrorType = decltype(getErrorType<Traits, Box>())::type;

template <typename V, template <typename...> class Box, typename T, typename... Args>
auto metaRebindValue(Box<T, Args...>*) -> Box<V, Args...>;

template <typename Traits, typename Box, typename T>
consteval auto rebindValue() noexcept {
    if constexpr (requires { typename Traits::template rebind_value<T>; }) {
        return std::type_identity<typename Traits::template rebind_value<T>>{};
    } else if constexpr (requires { typename Box::template rebind_value<T>; }) {
        return std::type_identity<typename Box::template rebind_value<T>>{};
    } else if constexpr (requires { metaRebindValue<T>(static_cast<Box*>(nullptr)); }) {
        return std::type_identity<decltype(metaRebindValue<T>(static_cast<Box*>(nullptr)))>{};
    } else {
        static_assert(False<Box>, "Not able to find rebind_value");
    }
}

template <typename Traits, typename Box, typename T>
using RebindValue = decltype(rebindValue<Traits, Box, T>())::type;

template <typename V, template <typename...> class Box, typename T, typename E, typename... Args>
auto metaRebindError(Box<T, E, Args...>*) -> Box<T, V, Args...>;

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

template <typename Traits, typename Box>
consteval auto liftError() noexcept {
    if constexpr (requires { Traits::lift_error(); }) {
        return &Traits::lift_error;
    } else if constexpr (requires { Traits::lift_error(Traits::error()); }) {
        return [](auto&& e) -> Box { return Traits::lift_error(std::forward<decltype(e)>(e)); };
    } else if constexpr (requires { Traits::error(); }) {
        return []() -> Box { return {}; };
    } else {
        return [](auto&& e) -> Box { return std::forward<decltype(e)>(e); };
    }
};

// template <typename BaseTraits, typename Box>
// struct LiftError;

// template <typename BaseTraits, typename Box>
// requires requires {
// { BaseTraits::error() };
// { BaseTraits::lift_error() };
// } || requires {
// { BaseTraits::lift_error(BaseTraits::error()) };
// }
// struct LiftError<BaseTraits, Box> {

// inline constexpr Box operator()(auto &&e) noexcept
// {
// return BaseTraits::lift_error()
// }
// };

// Traits::lift_error()        - FnPtr
// Traits::lift_error(error)   - FnPtr
// Box{}                       - lambda
// Box{error}                  - lambda

// template <typename BaseTraits>
// struct LiftError2 {

// template<>
// [[nodiscard]] inline constexpr operator()() const noexcept {

// }
// };

} // namespace details::_box_traits_builder

template <typename BaseTraits, typename Box>
struct BoxTraitsBuilder {
    using box_type = Box;

    using value_type = details::_box_traits_builder::ValueType<BaseTraits, Box>;

    template <typename T>
    using rebind_value = details::_box_traits_builder::RebindValue<BaseTraits, Box, T>;

    using error_type = details::_box_traits_builder::ErrorType<BaseTraits, Box>;

    template <typename E>
    using rebind_error = details::_box_traits_builder::RebindError<BaseTraits, Box, E>;

    inline static constexpr auto lift_error = details::_box_traits_builder::liftError<BaseTraits, Box>();

    // inline static constexpr Box lift_error() noexcept
    // requires (!(requires { BaseTraits::lift_error();  }))
    // {
    // return {};
    // }
};

template <instance_of<std::shared_ptr> Box>
struct beman::monadics::box_traits<Box> : BaseBoxTraits<Box>, BoxTraitsBuilder<BaseBoxTraits<Box>, Box> {

    // [[nodiscard]] inline static constexpr Box lift_error() noexcept {
    // return {};
    // }
};

// template <instance_of<std::shared_ptr> Box>
// struct beman::monadics::box_traits<Box> : BoxTraitsBuilder<box_traits<Box>, Box>
// {
// [[nodiscard]] inline static constexpr bool has_value(const Box& box) noexcept {
// return static_cast<bool>(box);
// }

// [[nodiscard]] inline static constexpr decltype(auto) value(auto&& box) noexcept {
// return *std::forward<decltype(box)>(box);
// }

// [[nodiscard]] inline static constexpr auto error() noexcept { return nullptr; }

// [[nodiscard]] inline static constexpr Box lift(auto &&value) noexcept {
// return std::make_shared<typename details::ValueType<Box>::value_type>(std::forward<decltype(value)>(value));
// }

// [[nodiscard]] inline static constexpr Box lift_error() noexcept {
// return {};
// }
// };

} // namespace beman::monadics

// error_type is deduced from error fn if error has 0 args.
// value_type is deduced as first template argument.
// rebind_error is deduced as same type if error fn has 0 args.
// rebind_value by default just change first args.
// lift_error does not exist, then ctor is used with error().
// struct beman::monadics::box_traits<Box> {
// [[nodiscard]] inline static constexpr bool has_value(const Box& box) noexcept {
// return static_cast<bool>(box);
// }

// [[nodiscard]] inline static constexpr decltype(auto) value(Box&& box) noexcept {
// return *std::forward<decltype(box)>(box);
// }

// [[nodiscard]] inline static constexpr Box lift(auto &&v) noexcept {
// return std::make_shared<typename Box::element_type>(std::forward<v>(v));
// }

// [[nodiscard]] inline static constexpr auto error() noexcept { return nullptr; }
// };

// #include "helpers/shared-ptr-full.hpp"

namespace beman::monadics::tests {

TEST_CASE("has-box-traits") {
    using Box     = std::shared_ptr<int>;
    using BT      = BaseBoxTraits<Box>;
    using Builder = BoxTraitsBuilder<BT, Box>;

    STATIC_REQUIRE(std::same_as<Builder::value_type, int>);
    STATIC_REQUIRE(std::same_as<Builder::error_type, std::nullptr_t>);

    STATIC_REQUIRE(std::same_as<Builder::rebind_value<float>, std::shared_ptr<float>>);
    STATIC_REQUIRE(std::same_as<Builder::rebind_error<float>, std::shared_ptr<int>>);
    // STATIC_REQUIRE(std::same_as<Builder::error_type, std::nullptr_t>);

    STATIC_REQUIRE(has_box_traits<std::shared_ptr<int>>);
}

TEST_CASE("with-value") {
    const auto result =
        std::make_shared<int>(10) | and_then([](auto&& value) { return std::make_shared<double>(value * 2.0); });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::shared_ptr<double>>);
    REQUIRE(static_cast<bool>(result));
    REQUIRE(*result == 20.0);
}

TEST_CASE("with-nullptr") {
    const auto result =
        std::shared_ptr<int>{} | and_then([](auto&& value) { return std::make_shared<double>(value * 1.0); });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::shared_ptr<double>>);
    REQUIRE(static_cast<bool>(result) == false);
}

struct Foo {};

struct Boo {
    std::shared_ptr<Foo> foo;
};

TEST_CASE("with-value-return-member") {
    const auto result =
        std::make_shared<Boo>() | and_then([](auto&& boo) { return std::forward<decltype(boo)>(boo).foo; });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::shared_ptr<Foo>>);
    REQUIRE(static_cast<bool>(result) == false);
}

} // namespace beman::monadics::tests
