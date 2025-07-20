// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// #include <beman/monadics/lift.hpp>

#include <gtest/gtest.h>

// #include <expected>

#include <concepts>
#include <optional>
#include <type_traits>
#include <utility>


namespace beman::monadics {

template<typename T>
inline constexpr bool is_optional_impl = false;

template<typename T>
inline constexpr bool is_optional_impl<std::optional<T>> = true;

template<typename T>
concept is_optional = is_optional_impl<std::remove_cvref_t<T>>;

template <typename T>
void dump() {
  std::cout << __PRETTY_FUNCTION__ << std::endl;
}

template<typename T>
struct Monad;


template<typename T>
  requires (is_optional<T>)
struct Monad<T> {
  [[nodiscard]] static bool has_value(const auto &opt) noexcept {
    return static_cast<bool>(opt);
  };

  [[nodiscard]] static decltype(auto) value(auto &&opt) noexcept {
    return *std::forward<decltype(opt)>(opt);
  };

  [[nodiscard]] static decltype(auto) error(auto) noexcept {
    return std::nullopt;
  };
};

template <typename T>
concept Box = requires(T t) {
    { *t } -> std::convertible_to<typename T::value_type>;
    { static_cast<bool>(t) } -> std::convertible_to<bool>;
};

template <typename T>
struct monad_traits;


template<typename T>
inline constexpr auto box_error_as = [] {};

template<typename T>
inline constexpr auto box_error_as<std::optional<T>> = std::nullopt;


// unwrap(Box box) -> *box
// unwrap_error(Box box) -> expected.error(), unexpected???
//                       -> optional{std::nullopt}
//                       -> std::shared_ptr{std::nullptr}
//                       -> std::weak_ptr{std::nullptr}
//                       -> std::unique_ptr{std::nullptr}

/*
template<typename T, typename E>
inline constexpr auto box_error_as<std::expected<T, E>> = [] (auto &&exp) {
  return std::unexpected{std::forward<decltype(exp)>.error()};
};
*/

template<Box T>
struct monad_traits<T> {
  [[nodiscard]] static bool has_value(const auto &box) noexcept {
    return static_cast<bool>(box);
  };

  [[nodiscard]] static decltype(auto) value(auto &&box) noexcept {
    return *std::forward<decltype(box)>(box);
  };

  [[nodiscard]] static decltype(auto) error(auto &&box) noexcept {
    return box_error_as<T>(std::forward<decltype(box)>(box));
  };
};

template<typename T>
struct monad_traits<std::optional<T>> {
  [[nodiscard]] static bool has_value(const auto &box) noexcept {
    return static_cast<bool>(box);
  };

  [[nodiscard]] static decltype(auto) value(auto &&box) noexcept {
    return *std::forward<decltype(box)>(box);
  };

  // [[nodiscard]] static decltype(auto) error(auto &&box) noexcept {
    // return box_error_as(std::forward<decltype(box)>(box));
  // };
};

template<is_optional T>
decltype(auto) pure_as() {
  std::cout << __PRETTY_FUNCTION__ << std::endl;


  /*
   return Monad{
     .has_value = [] (const auto &opt) {
        return static_cast<bool>(opt);
     },
     .unwrap = [] <typename U> (U &&opt) {
        return std::forward<U>(opt).value();
     }
   }
  */

  return [] <typename U> (U &&opt) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    dump<decltype(std::forward<U>(opt).value())>();
    return std::forward<U>(opt).value();
  };
}

// template<typename T>
// struct Monad {
  // T value;
// };

// template<typename T>
// auto pure() {
  // return pure_as<T>();
// }

} // namespace beman::monadics

namespace beman::monadics::tests {

/*

template<is_unique_ptr T>
Monad monad_as() {
  // movable only
  return {
    .has_value = &T::operator bool;
    .value = &T::deref;
  }
}

template<is_optional T>
Monad monad_as() {
  return {
    .has_value = &T::operator bool;
    .value = &T::deref;
    .error = std::nullopt
  }
}

template<is_optional T>
Monad monad_as() {
  return {
    .has_value = &T::operator bool;
    .value = &T::deref;
    .error = std::nullopt
  }
}

transform(Monad &&m, auto &&fn) {
  using ReturnValue = decltype(fn(m));

  if (!m) {
    return Monad::rebind<ReturnValue>(m);
  }

  return std::invoke(std::forward<decltype(fn)>(fn), std::forward<decltype(m)>(m).value());
}

// able to change value_type

and_then(Monad auto &&m, auto &&fn) {
  if (!m) {
    return std::forward<decltype(m)>(m).error();
  }

  return std::invoke(std::forward<decltype(fn)>(fn), std::forward<decltype(m)>(m).value());
}

// no value
// std::unexpected{Expected::error()}
// std::optional{std::nullopt}
// std::shared_ptr{nullptr}
// std::unique_ptr{nullptr}
// std::weak_ptr{nullptr}

template<is_expected T, typename F>
constexpr auto and_then_as(T&& t, F&& f) noexcept
    -> details::_expected::and_then_return<decltype(t), decltype(f)> {
  if (!t.has_value()) {
    return unexpected{std::forward<T>(t).error()};
  }

  if constexpr (details::_expected::is_void<T>) {
    return std::invoke(std::forward<F>(f));
  } else {
    return std::invoke(std::forward<F>(f), std::forward<T>(t).value());
  }
}

*/

template <typename T>
decltype(auto) pure(T &&value) {
  std::cout << __PRETTY_FUNCTION__ << std::endl;

  using M = Monad<decltype(std::forward<T>(value))>;
  return M::value(std::forward<T>(value));
}

template <typename T, typename Fn>
decltype(auto) and_then(T &&t, Fn &&fn) noexcept {
  using M = Monad<decltype(std::forward<T>(t))>;
  using Result = std::invoke_result_t<decltype(fn), decltype(M::value(std::forward<T>(t)))>;

  if (!M::has_value(t)) {
    return Result{M::error(t)};
  }

  return std::invoke(std::forward<Fn>(fn), M::value(std::forward<T>(t)));
}

TEST(LiftTest, optional) {

  std::cout << "====START=====" << std::endl;

  const auto r2 = pure(std::optional{5});
  EXPECT_EQ(r2, 5);


  const auto r3 = and_then(std::optional{1}, [] (int v) {
      return std::optional{v * 2.0};
  });
  EXPECT_TRUE(r3);
  EXPECT_EQ(r3.value(), 2.0);

  // Monad<std::optional<

  auto opt = pure_as<std::optional<int>&&>();

  decltype(auto) r = opt(std::optional{10});
  // static_assert(std::same_as<decltype(r), int&&>);
  EXPECT_EQ(r, 10);

  std::cout << "=====END======" << std::endl;
  // EXPECT_FALSE(true);
  // auto result = 10 | lift;

  // auto result = std::optional{10} | lift;
    // // | and_then()
}

} // namespace beman::monadics::tests


namespace beman::v2::monadics {

namespace details::_unwrap {

struct fn {
  template<typename T>
  [[nodiscard]] friend constexpr decltype(auto) operator|(T &&box, fn) noexcept {
    return std::forward<decltype(std::forward<T>(box).value())>(std::forward<T>(box).value());
  }
};

} // namespace details::_unwrap

inline constexpr details::_unwrap::fn unwrap{};

static_assert((std::optional{10} | unwrap) == 10);

namespace details::_unwrap_or {

struct fn {

  template<typename U>
  struct action {
    U value;

    template<typename T, typename A>
    [[nodiscard]] friend constexpr decltype(auto) operator|(T &&box, A &&a) noexcept {
      if (box.has_value()) {
        return std::forward<decltype(std::forward<T>(box).value())>(std::forward<T>(box).value());
      }

      return std::forward<decltype(std::forward<A>(a).value)>(std::forward<A>(a).value);
    }
  };

  template<typename T>
  [[nodiscard]] constexpr decltype(auto) operator()(T &&value) const noexcept {
    return action<decltype(value)>(std::forward<T>(value));
  }
};

} // namespace details::_unwrap_or

inline constexpr details::_unwrap_or::fn unwrap_or{};

// [[nodisacrd]] decltype(auto) unwrap_as

static_assert((std::optional<int>{std::nullopt} | unwrap_or(14)) == 14);

namespace details::_unwrap_error {

struct fn {
  template<typename T>
  [[nodiscard]] friend constexpr decltype(auto) operator|(T &&box, fn) noexcept {
    if constexpr (requires { box.error(); }) {
      return std::forward<decltype(std::forward<T>(box).error())>(std::forward<T>(box).error());
    } else {
      return unwrap_error_as(std::forward<T>(box));
    }
  }
};

} // namespace details::_unwrap_error

inline constexpr details::_unwrap_error::fn unwrap_error{};

} // namespace beman::v2::monadics

namespace std {

template<typename T>
[[nodiscard]] constexpr decltype(auto) unwrap_error_as(const std::optional<T> &) noexcept {
  return std::nullopt;
}

} // namespace std

namespace beman::v2::monadics {

// static_assert(std::same_as<decltype(std::optional<int>{std::nullopt} | unwrap_error), const std::nullopt_t>);

} // namespace beman::v2::monadics


namespace beman::v3::monadics {

template<typename T>
concept False = false;

template<typename T>
struct monad_traits {
  static_assert(False<T>, "monad_traits is not defined for type T");
};

namespace details::_unwrap_error {

struct fn {
  template<typename T>
  [[nodiscard]] friend constexpr decltype(auto) operator|(T &&box, fn) noexcept {
      return monad_traits<std::decay_t<T>>::unwrap_error(std::forward<T>(box));
  }
};

} // namespace details::_unwrap_error

inline constexpr details::_unwrap_error::fn unwrap_error{};

template <typename T>
  requires requires (T t) {
    { unwrap_error_as(t) };
  }
struct monad_traits<T> {
  static decltype(auto) unwrap_error(const auto &t) {
    return unwrap_error_as(t);
  }
};


template <typename T>
struct box_traits;

template <typename T>
  requires requires (T t) {
    { static_cast<bool>(t) } -> std::same_as<bool>;
    { *t };
  }
struct box_traits<T> {
  [[nodiscard]] inline constexpr bool has_value(const auto &box) const noexcept {
    return static_cast<bool>(box);
  }

  [[nodiscard]] inline constexpr decltype(auto) value(auto &&box) const noexcept {
    return std::forward<decltype(*std::forward<T>(box))>(*std::forward<T>(box));
  }
};

template <typename T>
concept Box = requires (T t) {
  typename box_traits<T>;
};


// template<IsMyOptional T>
// auto box_as(T &&opt) {
//    return Box{
//      .value = T::get,
//      .has_value = T::isValid,
//    }
// }

template <typename T>
struct box_with_error_traits;

template <typename T>
struct box_with_error_traits : box_traits<T> {
  [[nodiscard]] inline constexpr decltype(auto) error(auto &&box) const noexcept {
    return std::forward<decltype(std::forward<decltype(box)>(box).error())>(std::forward<decltype(box)>(box).error());
  }
};

template <typename T>
concept box_with_error = requires {
  typename box_with_error_traits<T>;
};

template <Box T>
struct monad_traits<T> {
  static decltype(auto) unwrap(auto &&box) {
    return box_traits<T>::value(std::forward<decltype(box)>(box));
  }

  static decltype(auto) unwrap_or(auto &&box, auto &&d) {
    if (box_traits<T>::has_value(box)) {
        return box_traits<T>::value(std::forward<decltype(box)>(box));
    }

    return std::forward<decltype(d)>(d);
  }

  // static decltype(auto) unwrap_error(const auto &box) {
    // return monad_traits<T>::unwrap_error(box);
  // }
};

template <typename T>
concept Monad = requires (T t) {
  typename monad_traits<T>;
};

// template<typename T>
// struct box_traits;

// template<Box T>
// struct box_traits<T> {
  // static decltype(auto) unwrap_or(auto &&opt, auto) {
    // return std::forward<decltype(opt)>(opt);
  // }

  // static decltype(auto) unwrap_error(const auto &) {
    // return std::nullopt;
  // }
// };

// template <typename T>
// struct monad_traits<std::optional<T>> {
  // static decltype(auto) unwrap_or(auto &&opt, auto) {
    // return std::forward<decltype(opt)>(opt);
  // }

  // static decltype(auto) unwrap_error(const auto &) {
    // return std::nullopt;
  // }
// };

static_assert(std::same_as<decltype(std::optional<int>{std::nullopt} | unwrap_error), const std::nullopt_t>);

} // namespace beman::v2::monadics
