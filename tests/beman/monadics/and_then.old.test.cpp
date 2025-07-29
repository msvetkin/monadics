// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/box.hpp>

#include <concepts>
#include <gtest/gtest.h>

#include <optional>
#include <type_traits>
#include <utility>

namespace beman::monadics::tests {

template <typename T, typename U>
struct is_same_box2 : std::false_type {};

template <template <typename...> class Box, typename... Args1, typename... Args2>
struct is_same_box2<Box<Args1...>, Box<Args2...>> : std::true_type {};

template <typename T, typename U>
concept is_same_box = is_same_box2<T, U>::value;

static_assert(is_same_box<std::optional<int>, std::optional<float>>);
static_assert(!is_same_box<std::optional<int>, std::vector<int>>);
static_assert(!is_same_box<std::optional<int>, int>);

template <typename T, typename E>
struct Result {};

static_assert(is_same_box<Result<int, int>, Result<float, int>>);

template <typename Box>
using BoxValue = decltype(box_traits<Box>::value(std::declval<Box>()));

template <typename Box, typename Fn>
    requires std::invocable<Fn, BoxValue<Box>>
using NewBox = std::invoke_result_t<Fn, BoxValue<Box>>;

template <typename Box, typename Fn, typename R = NewBox<Box, Fn>>
    requires requires {
        requires is_same_box<Box, R>;
        // check error_type
    }
using ReturnBox = R;

template <typename Box, typename Fn>
constexpr auto and_then(Box box, Fn fn) -> ReturnBox<decltype(box), decltype(fn)> {
    using Traits = box_traits<Box>;
    if (!Traits::has_value(box)) {
        // return Traits::error(box);
        return std::nullopt;
    }

    return std::invoke(std::forward<Fn>(fn), Traits::value(std::forward<Box>(box)));
}

// box_traits::has_value - unwrap_or ?
// box_traits::value     - unwrap
// box_traits::error     - unwrap_error

// box_traits {
//    using box_type   =
//    using value_type =
//    using error_type =
//
//    using rebind_value = ?
//    using rebind_error = ?
//
//    value(box&&)
//    has_value(const box&)
//    error(box&&) | error()
// }
//
//
// box_as(T&&) {
//    return std::tuple{
//       &t.value
//    };
// }
//
//
// simple
// box_traits {
//    value(box_type&&)
//    has_value(const box_type&)
//    error(box_type&&) | error()
// }
//
// full
// box_traits {
//    value(box_type&&)
//    has_value(const box_type&)
//    error(box_type&&) | error()
//
//    using value_type = typename box_type::value_type or deduced from value
//    using error_type = typename box_type::error_type or deduced from error
//
//    using rebind_value = typename box_type::rebind_value
//    using rebind_error = typename box_type::rebind_error
//
// }
//
//
//
// weakWidget
//  | or_else([] { return std::unexpected{"widget is null"}; })
//  | and_then(&Widget::getView)
//  | or_else([] { return std::unexpected{"view is null"}; })
//
// weakWidget
//  | lift_to<std::shared_ptr>
//  | and_then(&Widget::getView)
//
//  1. lock
//  2. pass pointer
//  3. return shared_ptr

TEST(AndThenTest, sanity) {
    constexpr auto result = and_then(std::optional{10}, [](int value) {
        return std::make_optional(value * 2.0);
        // return value * 2.0;
    });
    static_assert(result.value() == 20.0);
    // int&& (std::optional<int>::*get_value_rvalue_ref)() && = &std::optional<int>::value;
}

template <typename Opt>
struct optional_value_ptr;

// Specializations based on ref-qualification
template <typename T>
struct optional_value_ptr<T&> {
    using value = typename T::value_type&;
    using type  = value (T::*)() &;
};

template <typename T>
struct optional_value_ptr<T&&> {
    using value = typename T::value_type&&;
    using type  = value (T::*)() &&;
};

template <typename T>
struct optional_value_ptr<const T&> {
    using value = const typename T::value_type&;
    using type  = value (T::*)() const&;
};

template <typename T>
struct optional_value_ptr<const T&&> {
    using value = const typename T::value_type&&;
    using type  = value (T::*)() const&&;
};

// Shorthand alias
template <typename Opt>
using optional_value_ptr_t = typename optional_value_ptr<Opt>::type;

template <typename T>
struct Getter {
    // int (std::optional<int>::*value)() const &&;
    optional_value_ptr_t<T> get;
};

template <typename T>
struct getter_fn;

template <typename T>
struct getter_fn<T&> {
    using type = typename T::value_type& (T::*)() &;
};

template <typename T>
struct value_fn;

template <typename T>
struct value_fn<T&> {
    using type = typename T::value_type& (T::*)() &;
};

template <typename T>
struct value_fn<const T&> {
    using type = const typename T::value_type& (T::*)() const&;
};

template <typename T>
struct value_fn<T&&> {
    using type = typename T::value_type && (T::*)() &&;
};

template <typename T>
struct value_fn<const T&&> {
    using type = const typename T::value_type && (T::*)() const&&;
};

template <typename T>
using value_fn_t = typename value_fn<T>::type;

template <typename T>
struct has_value_fn;

template <typename T>
struct has_value_fn<T&> {
    using type = bool (T::*)() const;
};

template <typename T>
struct has_value_fn<const T&> {
    using type = bool (T::*)() const;
};

template <typename T>
struct has_value_fn<T&&> {
    using type = bool (T::*)() const;
};

template <typename T>
struct has_value_fn<const T&&> {
    using type = bool (T::*)() const;
    // using type = const bool (T::*)() const &&;
};

template <typename T>
using has_value_fn_t = typename has_value_fn<T>::type;

/*
template <typename T>
struct value_fn {
    using type = attribute_traits<T>::
    const typename T::value_type&& (T::*)() const &&;
};
*/

template <typename T>
struct MyBox {
    using box_type = typename std::remove_cvref_t<T>;

    value_fn_t<T>     value;
    has_value_fn_t<T> has_value;
};

template <typename T>
inline constexpr auto is_optional2 = false;

template <typename T>
inline constexpr auto is_optional2<std::optional<T>> = true;

template <typename T>
concept is_optional = is_optional2<std::remove_cvref_t<T>>;

/*

template<typename T>
concept is_optional = requires {
  typename std::remove_cvref_t<T>::value_type;
  requires std::same_as<T, std::optional<typename std::remove_cvref_t<T>::value_type>;
}

*/

template <typename T, template <typename...> typename U>
struct is_same_box3 {
    inline static constexpr bool value = false;
};

template <template <typename...> class Box, template <typename...> class Box2, typename... Args1>
struct is_same_box3<Box<Args1...>, Box2> {
    inline static constexpr bool value = requires {
        typename Box2<Args1...>;
        requires std::same_as<Box<Args1...>, Box2<Args1...>>;
    };
};

template <typename B1, template <typename...> class B2>
concept same_box = is_same_box3<B1, B2>::value;

namespace v2 {

template <typename T1, template <typename...> typename T2>
struct is_same_type {
    inline static constexpr bool value = false;
};

template <template <typename...> class T1, typename... Args1, template <typename...> class T2>
struct is_same_type<T1<Args1...>, T2> {
    inline static constexpr bool value = requires {
        typename T2<Args1...>;
        requires std::same_as<T1<Args1...>, T2<Args1...>>;
    };
};

template <typename B1, template <typename...> class T2>
concept is_box_type = requires {
    // requires IsBox<B1>;
    requires is_same_type<typename B1::box_type, T2>::value;
};

static_assert(is_box_type<MyBox<std::optional<int>&>, std::optional>);

} // namespace v2

// static_assert(same_box<std::optional<int>, std::vector>);

template <v2::is_box_type<std::optional> BoxTraits>
constexpr BoxTraits as_box() {
    using T = BoxTraits::box_type;

    return {
        .value     = &T::value,
        .has_value = &T::operator bool,
    };
}

template <typename T>
consteval std::string_view dump() {
    return __PRETTY_FUNCTION__;
};

template <typename Expected, typename T>
auto as_box_tester(T&& opt) {
    using Box = MyBox<decltype(opt)>;

    constexpr auto box   = as_box<Box>();
    decltype(auto) value = std::invoke(box.value, std::forward<T>(opt));
    std::cout << "value: " << value << ", type: " << dump<decltype(value)>() << std::endl;

    static_assert(std::same_as<Expected, decltype(value)>);
    EXPECT_EQ(value, 10);
}

TEST(AndThenTest, opti) {
    {
        std::optional opt{10};
        as_box_tester<int&>(opt);
        // EXPECT_FALSE(true);
    }

    {
        const std::optional opt{10};
        as_box_tester<const int&>(opt);
        // EXPECT_FALSE(true);
    }

    {
        std::optional opt{10};
        as_box_tester<int&&>(std::move(opt));
        // EXPECT_FALSE(true);
    }

    {
        const std::optional opt{10};
        as_box_tester<const int&&>(std::move(opt));
        // EXPECT_FALSE(true);
    }

    // using Opt = std::optional<int>&&;
    // using Ptr = int& (std::optional<int>::*)() &;

    // auto v = static_cast<Ptr>(&std::optional<int>::value);
    // optional_value_ptr_t<Opt> get_value = &std::optional<int>::value;
    // std::optional<int> v1;
    // MyBox<decltype(v1)> box{.value = &std::optional<int>::value};

    // constexpr auto box = make_box<std::optional<int>&&>();

    // [](auto &&b) {
    // optional_value_ptr_t<decltype(b)> get_value = &std::optional<int>::value;
    // }(std::optional{10});

    // You know the type is rvalue reference (std::optional<int>&&)
    // Getter getter{&std::optional<int>::value};
}

template <typename T>
struct MyOptional {
    using value_type = T;

    T& get() & { return v; }

    const T& get() const& { return v; }

    inline operator bool() const { return true; }

    T v{15};
};

template <v2::is_box_type<MyOptional> BoxTraits>
constexpr BoxTraits as_box() {
    using T = BoxTraits::box_type;

    return {
        .value     = &T::get,
        .has_value = &T::operator bool,
    };
}

/*

template<v2::is_same_type<MyOptional> T>
struct box_traits<T> {

  template<typename U>
  static inline consteval auto as_box() const noexcept {
    return Box<U>{
      .value = &T::get,
      .has_value = &T::operator bool,
    };
  }
};

*/

template <typename ValuePtr, typename HasValuePtr>
struct Box23 {
    ValuePtr    value;
    HasValuePtr has_value;
};

template <typename R, typename T>
constexpr auto helper(R (T::*ptr)()) noexcept -> decltype(ptr) {
    return ptr;
}

template <typename R, typename T>
constexpr auto helper(R (T::*ptr)() const) noexcept -> decltype(ptr) {
    return ptr;
}

// template <typename ValuePtr, typename HasValuePtr>
// Box23(ValuePtr, HasValuePtr)

TEST(AndThenTest, opti2) {
    {
        using T = MyOptional<int>;

        // auto value = [] (auto &&opt) {
        // return std::forward<T>(opt).get();
        // };

        // auto has_value = [] (auto &&opt) {
        // return static_cast<bool>(opt.get());
        // };

        // constexpr auto ff = Box23{
        // &T::get,
        // &T::operator bool,
        // };

        MyOptional<int> opt{};
        // as_box_tester<int&>(opt);
        // EXPECT_FALSE(true);
    }
}

} // namespace beman::monadics::tests
