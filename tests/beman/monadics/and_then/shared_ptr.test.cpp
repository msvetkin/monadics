// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "beman/monadics/detail/and_then.hpp"

#include <catch2/catch_test_macros.hpp>
#include <memory>


namespace beman::monadics {

namespace details::_instance_of {

template <typename t, template <typename...> class u>
inline constexpr bool same_as = false;

template <template <typename...> class t, typename... args, template <typename...> class u>
inline constexpr bool same_as<t<args...>, u> = requires { requires std::same_as<t<args...>, u<args...>>; };

} // namespace details::_instance_of

namespace details {

template<typename T>
concept has_value_type = requires {
  typename T::value_type;
};

template<typename T>
struct extract_value_type;

// template<has_value_type T>
// struct extract_value_type<T> {
  // using type = typename T::value_type;
// };

template<template<typename ...> typename T,
         typename V,
         typename ... Args
        >
  // requires (!has_value_type<T<V, Args...>>)
struct extract_value_type<T<V, Args...>> {
  using type = V;
};

template<typename Box>
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

template<typename Box, typename V>
using RebindValue = details::_rebind_value::type<std::remove_cvref_t<Box>, std::remove_cvref_t<V>>;

} // namespace details

namespace details::_error_type {

} // namespace details::_error_type

namespace details {

template<typename BoxTraits>
struct ErrorType;

template<typename BoxTraits>
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

template<typename Box>
struct BaseBoxTraits;

template <instance_of<std::shared_ptr> Box>
struct BaseBoxTraits<Box>
{
    [[nodiscard]] inline static constexpr bool has_value(const Box& box) noexcept {
      return static_cast<bool>(box);
    }

    [[nodiscard]] inline static constexpr decltype(auto) value(auto&& box) noexcept {
        return *std::forward<decltype(box)>(box);
    }

    [[nodiscard]] inline static constexpr auto error() noexcept { return nullptr; }
};

namespace details {

namespace details::_rebind_error {

template<typename Box>
struct SameBox {
  template <typename>
  using rebind_error = Box;
};

} // namespace details::_rebind_error

template<typename BoxTraits, typename Box, typename E>
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

} // namespace


template <instance_of<std::shared_ptr> Box>
struct beman::monadics::box_traits<Box> : BaseBoxTraits<Box>,
                                          details::ValueType<Box>
                                          // details::RebindValue<Box>
{
    // using value_type = typename Box::element_type;
    using error_type = typename details::ErrorType<BaseBoxTraits<Box>>::error_type;

    template <typename V>
    using rebind_value = details::RebindValue<Box, V>;

    template <typename E>
    using rebind_error = decltype(details::rebindError<BaseBoxTraits<Box>, Box, E>())::type;

    // lift
    [[nodiscard]] inline static constexpr Box lift(auto &&value) noexcept {
      if constexpr (std::same_as<std::remove_cvref_t<decltype(value)>, std::nullptr_t>) {
          return {};
      } else {
          return std::make_shared<typename details::ValueType<Box>::value_type>(std::forward<decltype(value)>(value));
      }
    }
};

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
    STATIC_REQUIRE(has_box_traits<std::shared_ptr<int>>);
}

TEST_CASE("with-value") {
  const auto result = std::make_shared<int>(10)
      | and_then([] (auto &&value) {
          return std::make_shared<double>(value * 2.0);
      });
  STATIC_REQUIRE(std::same_as<decltype(result), const std::shared_ptr<double>>);
  REQUIRE(static_cast<bool>(result));
  REQUIRE(*result == 20.0);
}

TEST_CASE("with-nullptr") {
  const auto result = std::shared_ptr<int>{}
      | and_then([] (auto &&value) {
          return std::make_shared<double>(value * 1.0);
      });
  STATIC_REQUIRE(std::same_as<decltype(result), const std::shared_ptr<double>>);
  REQUIRE(static_cast<bool>(result) == false);
}

struct Foo {};

struct Boo {
  std::shared_ptr<Foo> foo;
};

TEST_CASE("with-value-return-member") {
  const auto result = std::make_shared<Boo>()
      | and_then([] (auto &&boo) {
          return std::forward<decltype(boo)>(boo).foo;
      });
  STATIC_REQUIRE(std::same_as<decltype(result), const std::shared_ptr<Foo>>);
  REQUIRE(static_cast<bool>(result) == false);
}

} // namespace beman::monadics::tests
