// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "beman/monadics/box.hpp"
#include <beman/monadics/type_traits/box.hpp>
#include <beman/monadics/concepts/instance_of.hpp>

#include <catch2/catch_test_macros.hpp>

#include <optional>
#include <type_traits>
#include <utility>
#include <functional>

namespace mm {

template <typename T>
struct Optional {
    using Value = T;

    T value{-1};

    constexpr bool valid() const { return value != -1; }

    constexpr T get() const { return value; }
};

// template<instance_of<Optional> T>
// struct box_traits<T> {

// }

// template<beman::monadics::is_box_of<Optional> T>
// constexpr decltype(auto) box_as(T) noexcept {
// using U = typename T::type;

// return std::tuple{
// &U::valid,
// &U::get
// };
// }

} // namespace mm

template <beman::monadics::instance_of<mm::Optional> T>
struct beman::monadics::box_traits<T> {
    // using Box = std::remove_cvref_t<T>;
    // rebind_value
    // rebind_error

    inline static constexpr bool has_value(const T& box) noexcept { return box.valid(); }

    template <typename U>
    inline static constexpr decltype(auto) value(U&& box) noexcept {
        return std::forward<U>(box).get();
        // return std::forward<decltype(std::forward<U>(box).get())>(std::forward<U>(box).get());
    }

    inline static constexpr decltype(auto) error() noexcept { return -1; }
};

/*


template<beman::monadics::instance_of<std::optional> T>
struct beman::monadics::box_traits<T> : std_box_traits<T, std::nullopt> {};

template<beman::monadics::instance_of<std::expected> T>
struct beman::monadics::box_traits<T> : std_box_traits<T> {};

template<beman::monadics::instance_of<std::expected> T>
struct beman::monadics::box_traits<T> : std_box_traits<T> {};

template<beman::monadics::instance_of<optional> T>
constexpr auto box_as(box_tag<T>) -> BoxTraits<T::value_type> noexcept {
  return {
    .has_value = &T::operator bool,
    .value = &T::value,
    .error = std::nullopt
  };
}

template<beman::monadics::is_box_of<optional> T>
constexpr auto box_as(T) -> T noexcept {
  return {
    .has_value = &T::operator bool,
    .value = &T::value,
    .error = std::nullopt
  };
}

template<beman::monadics::is_box_of<optional> T>
constexpr auto box_as(T) -> T noexcept {
  return {
    .has_value = &T::operator bool,
    .value = &T::value,
    .error = std::nullopt
  };
}

template<beman::monadics::is_box_of<optional> T>
constexpr auto box_as(T) noexcept {
  return T::template Result<typename T::value_type, std::nullopt_t>{
    .has_value = &T::operator bool,
    .value = &T::value,
    .error = std::nullopt
  };
}

template<beman::monadics::is_box_of<optional> T>
constexpr auto box_as(beman::monadics::tag<T>) noexcept {
  typename U = std::remove_cvref_t<T>;

  return beman::monadics::Box<typename U::value_type>{
    .has_value = &U::operator bool,
    .value = &U::value,
    .error = std::nullopt
  };
}

template<beman::monadics::is_box_of<optional> T>
constexpr decltype(auto) has_value(const T &t) noexcept {
  return t.has_value();
}

template<beman::monadics::is_box_of<optional> T>
constexpr decltype(auto) unwrap_as(T &&t) noexcept {
  return std::forward<T>(t).value();
}

template<beman::monadics::is_box_of<optional> T>
constexpr decltype(auto) unwrap_error() noexcept {
  return std::nullopt;
}


*/

namespace beman::monadics::tests {

TEST_CASE("optional") {
    constexpr mm::Optional<int> opt{10};
    using Traits = box_traits_for<decltype(opt)>;
    STATIC_REQUIRE(Traits::has_value(opt));
    STATIC_REQUIRE(Traits::value(opt) == 10);

    // constexpr box_traits<mm::Optional<int>> traits{};

    // constexpr auto wrapper = traits.wrapper<decltype(opt)>();
    // STATIC_REQUIRE(std::invoke(std::get<0>(wrapper), opt) == false);
    // using Opt = rebind_value<std::optional<int>, float>;
    // STATIC_REQUIRE(std::same_as<Opt, std::optional<float>>);
}

struct Foo {
    constexpr int value() const& { return 10; }

    constexpr int value() && { return 20; }
};

template <typename T>
struct BTraits {
    using type = T;

    using value_type = typename T::value_type; // mandotary
    using error_type = typename T::error_type; // optional

    // has_value_fn<T> has_value;
    // bool (T::*has_value)() const;
    //
    // 4, 2, 1
    // value_fn<T>     value;
    // error_fn<T>     error;
};

TEST_CASE("fdsf") {

    constexpr Foo foo;

    auto ptr = static_cast<int (Foo::*)() const&>(&Foo::value);

    // static_cast<int (Foo::*)() const &>(&Foo::value)
    // static_cast<X (Foo::*)() Y>(&Foo::value)
    // decltype(f.value())

    auto r = std::invoke(ptr, foo) == 10;
}

/*

template<beman::monadics::is_box_of<Optional> T>
constexpr decltype(auto) box_as(T) noexcept {
  using U = typename T::type;

  return Wrapper<typename U::value_type>{
  {
    &U::valid,
    &U::get
  };
}

*/

} // namespace beman::monadics::tests
