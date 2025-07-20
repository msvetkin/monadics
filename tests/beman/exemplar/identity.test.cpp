// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/exemplar/identity.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <functional>

namespace exe = beman::exemplar;

TEST(IdentityTest, call_identity_with_int) {
    for (int i = -100; i < 100; ++i) {
        EXPECT_EQ(i, exe::identity()(i));
    }
}

TEST(IdentityTest, call_identity_with_custom_type) {
    struct S {
        int i;
    };

    for (int i = -100; i < 100; ++i) {
        const S s{i};
        const S s_id = exe::identity()(s);
        EXPECT_EQ(s.i, s_id.i);
    }
}

TEST(IdentityTest, compare_std_vs_beman) {
// Requires: std::identity support.
#if defined(__cpp_lib_type_identity)
    std::identity std_id;
    exe::identity beman_id;
    for (int i = -100; i < 100; ++i) {
        EXPECT_EQ(std_id(i), beman_id(i));
    }
#endif
}

TEST(IdentityTest, check_is_transparent) {
// Requires: transparent operators support.
#if defined(__cpp_lib_transparent_operators)

    exe::identity id;

    const auto container = {1, 2, 3, 4, 5};
    auto       it        = std::find(std::begin(container), std::end(container), 3);
    EXPECT_EQ(3, *it);
    auto it_with_id = std::find(std::begin(container), std::end(container), id(3));
    EXPECT_EQ(3, *it_with_id);

    EXPECT_EQ(it, it_with_id);
#endif
}


/*

  std::expected<int, std::string> parseInt(std::string);
  int squared(int);

  expected<int, std::string> result =
    getValue()               // std::expected<std::string, std::string>
      | unwrap               // std::string
      | transform(parseInt)  // std::expected<int, std::string>
      | transform(squared)   // int


  std::expected<int, std::string> result =
    getValue()               // std::optional<std::string>
      | transform(parseInt)  // std::expected<int, std::string>
                             // std::optional<std::expected<int, std::string>>
      | transform(squared)   // int
                             // std::optional<int>
      | unwrap               // std::expected<int, string>


  std::expected<int, std::string> result =
    getValue()
      | transform(parseInt)
      | transform(squared)
      | unwrap_or(std::string{})

  std::expected<int, std::string> result =
    getValue()
      | transform(parseInt)
      | transform(squared)
      | transform_error // constructs default error std::string if possible
      | transform_error(std::nullopt -> std::string) ?

  auto unwrap_as(std::optional<T> m) {
    return m.value();
  }

  auto unwrap_error_as(const std::optional<T> &) {
    return std::nullopt;
  }

  auto unwrap_as(std::expected<T, E> m) {
    return m.value();
  }

  auto unwrap_error_as(std::expected<T, E> m) {
    return m.error();
  }

  auto unwrap_as(T *m) {
    return *m;
  }

  auto unwrap_error_as(T *m) {
    return nullptr;
  }

  auto monad_as(std::identity_t<T>) {
    return Monad{
      unwrap_as([] (auto &&t) { return t.value()}),
      unwrap_error_as([] (auto &&t) { return t.value()}),
    };
  }

  template<typename T, typename E>
  struct Monad<std::expected<T, E>> {
    decltype(auto) value(auto &&exp) const noexcept {
      return std::forward<decltype(exp)>(exp).value();
    }

    decltype(auto) error(auto &&exp) const noexcept {
      return std::forward<decltype(exp)>(exp).error();
    }
  };

  std::expected<int, std::string> result =
    getValue()
      | pure                  -> Monad<optional<int>>
      | transform(parseInt)   -> Monad<expected<int, Error>>
      | transform(squared)    -> Monad<expected<int, Error>>
      | unwrap                -> expected<int, Error>

  // looks douable optional to expected
  std::expected<int, std::string> result =
    getValue()                -> optional<int>
      | transform(parseInt)   -> optional<expected<int, Error>>
      | unwrap/unwrap_or      -> expected<int, Error>
      | transform(squared)    -> expected<int, Error>

  // looks douable optional to expected, unwrap_or version
  std::expected<int, std::string> result =
    getValue()                -> optional<int>
      | transform(parseInt)   -> optional<expected<int, Error>>
      | unwrap_or<Error>      -> expected<int, Error>
      | transform(squared)    -> expected<int, Error>

  None(int) -> None(expected) -> Result<T, Error> ->

  // looks douable expected to optional
  std::optional<int> result =
    getValue()                -> expected<int, Error>
      | transform(parseInt)   -> expected<optional<int>, Error>>
      | unwrap/unwrap_or      -> optional<int>
      | transform(squared)    -> optional<int>

  std::optional<int> result =
    getValueOr()            -> expected<T, E>
      | transform(parseInt) -> optional<T -> U>
      | transform(squared)  -> int
      | or_else(print)      -> void(E)

  T,E -> Some(U) -> U -> Some(U) or print E


  Monad(T) -> void -> Monad(T)
  Monad(T) -> U    -> Monad(U)

  expected<vector<int>, error> result =
    getValues()          // expected<vector<int>, std::string>
      | unwrap           // std::vector
      | and_then(print)  // std::vector
      | map(divide, 10)  //
      | and_then(print)
 *
*/
