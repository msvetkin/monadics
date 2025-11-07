// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cstddef>
#include <tuple>
#include <type_traits>
struct Foo {
    int value{};
};
struct Boo {
    Foo* foo{nullptr};
};

#define BEMAN_MONADICS_BOX_TRAITS_BUILDER 1
#include "beman/monadics/detail/and_then.hpp"

#include <catch2/catch_test_macros.hpp>

#include "helpers/raw-ptr-full.hpp"

namespace beman::monadics::tests {

TEST_CASE("has-box-traits") {
    using Traits = box_traits_for<int*>;
    STATIC_REQUIRE(std::same_as<Traits::value_type, int>);
    STATIC_REQUIRE(std::same_as<Traits::error_type, std::nullptr_t>);
    STATIC_REQUIRE(std::same_as<Traits::rebind_value<float>, float*>);
    STATIC_REQUIRE(std::same_as<Traits::rebind_error<float>, int*>);
    int value = 10;
    REQUIRE(Traits::value(&value) == 10);
    STATIC_REQUIRE(Traits::error() == nullptr);
    // STATIC_REQUIRE(Traits::lift(10) == std::optional{10});
    //
    //
    //
    auto getValue = [](auto&& b) -> decltype(Traits::value(std::forward<decltype(b)>(b))) {
        // return Traits::value(std::forward<decltype(b)>(b));
        //
        // using R = decltype(Traits::value2(std::forward<decltype(b)>(b)));
        // static_assert(std::same_as<R, int&>);
        // return std::forward<R>(Traits::value2(std::forward<decltype(b)>(b)));
        return Traits::value(std::forward<decltype(b)>(b));
    };

    STATIC_REQUIRE(std::same_as<decltype(getValue(std::move(static_cast<int*>(nullptr)))), int&>);

    STATIC_REQUIRE(Traits::lift_error() == static_cast<int*>(nullptr));
}

TEST_CASE("with-value") {
    constexpr int value = []() {
        Foo foo{.value = 5};
        Boo boo{.foo = &foo};

        auto result = &boo | and_then([&](auto&& b) {
            // static_assert(std::same_as<decltype(b), Boo*&>);
            // return std::addressof(*b);
            // b.foo->value += 5;
            return b.foo;
        }) | and_then([](Foo& f) {
            f.value += 5;
            return &f;
        });

        return result->value;
    }();

    STATIC_REQUIRE(value == 10);
}

TEST_CASE("self2") {
    constexpr int value = []() {
        Foo foo{.value = 5};
        Boo boo{.foo = &foo};

        auto result = &boo | and_then([](auto&& b) { return &b; }) | and_then([](auto&& b) { return &b; });
        return result->foo->value;
    }();

    STATIC_REQUIRE(value == 5);
}

template <typename T1, typename T2>
auto inputQualifiers2(T1 t1, T2&& t2) {
    using Tuple = std::tuple<T1, decltype(std::forward<T1>(t1)), T2, decltype(std::forward<T2>(t2))>;
    return std::type_identity<Tuple>{};
};

template <typename T1, typename T2>
auto inputQualifiers(T1 t1, T2&& t2) {
    using Tuple1 = decltype(inputQualifiers2(t1, std::forward<T2>(t2)))::type;

    using Tuple = std::tuple<T1,
                             decltype(std::forward<T1>(t1)),
                             T2,
                             decltype(std::forward<T2>(t2)),
                             std::tuple_element_t<0, Tuple1>,
                             std::tuple_element_t<1, Tuple1>,
                             std::tuple_element_t<2, Tuple1>,
                             std::tuple_element_t<3, Tuple1> >;
    return std::type_identity<Tuple>{};
};

TEST_CASE("input-qualifiers") {
    struct Foo {};

    {
        // template args - deduced
        // passed        - temporary
        using Tuple      = decltype(inputQualifiers(Foo{}, Foo{}))::type;
        using T1         = std::tuple_element_t<0, Tuple>;
        using ForwardT1  = std::tuple_element_t<1, Tuple>;
        using TA1        = std::tuple_element_t<4, Tuple>;
        using ForwardTA1 = std::tuple_element_t<5, Tuple>;

        using T2         = std::tuple_element_t<2, Tuple>;
        using ForwardT2  = std::tuple_element_t<3, Tuple>;
        using TA2        = std::tuple_element_t<6, Tuple>;
        using ForwardTA2 = std::tuple_element_t<7, Tuple>;

        static_assert(std::same_as<T1, Foo>);
        static_assert(std::same_as<ForwardT1, Foo&&>);
        static_assert(std::same_as<TA1, Foo>);
        static_assert(std::same_as<ForwardTA1, Foo&&>);

        static_assert(std::same_as<T2, Foo>);
        static_assert(std::same_as<ForwardT2, Foo&&>);
        static_assert(std::same_as<TA2, Foo>);
        static_assert(std::same_as<ForwardTA2, Foo&&>);
    };

    {
        // template args - T1 implicit, T2 deduced
        // passed        - temporary
        using Tuple      = decltype(inputQualifiers<Foo&&>(Foo{}, Foo{}))::type;
        using T1         = std::tuple_element_t<0, Tuple>;
        using ForwardT1  = std::tuple_element_t<1, Tuple>;
        using TA1        = std::tuple_element_t<4, Tuple>;
        using ForwardTA1 = std::tuple_element_t<5, Tuple>;

        using T2         = std::tuple_element_t<2, Tuple>;
        using ForwardT2  = std::tuple_element_t<3, Tuple>;
        using TA2        = std::tuple_element_t<6, Tuple>;
        using ForwardTA2 = std::tuple_element_t<7, Tuple>;

        static_assert(std::same_as<T1, Foo&&>);
        static_assert(std::same_as<TA1, Foo>);
        static_assert(std::same_as<ForwardT1, Foo&&>);
        static_assert(std::same_as<ForwardTA1, Foo&&>);

        static_assert(std::same_as<T2, Foo>);
        static_assert(std::same_as<ForwardT2, Foo&&>);
    };

    {
        // template args - deduced
        // passed        - as value
        Foo foo;
        using Tuple     = decltype(inputQualifiers(foo, foo))::type;
        using T1        = std::tuple_element_t<0, Tuple>;
        using ForwardT1 = std::tuple_element_t<1, Tuple>;

        using T2        = std::tuple_element_t<2, Tuple>;
        using ForwardT2 = std::tuple_element_t<3, Tuple>;

        static_assert(std::same_as<T1, Foo>);
        static_assert(std::same_as<ForwardT1, Foo&&>);

        static_assert(std::same_as<T2, Foo&>);
        static_assert(std::same_as<ForwardT2, Foo&>);
    };

    {
        // template args - T1 implicit, T2 deduced
        // passed        - as value
        Foo foo;
        using Tuple     = decltype(inputQualifiers<Foo&>(foo, foo))::type;
        using T1        = std::tuple_element_t<0, Tuple>;
        using ForwardT1 = std::tuple_element_t<1, Tuple>;

        using T2        = std::tuple_element_t<2, Tuple>;
        using ForwardT2 = std::tuple_element_t<3, Tuple>;

        static_assert(std::same_as<T1, Foo&>);
        static_assert(std::same_as<ForwardT1, Foo&>);

        static_assert(std::same_as<T2, Foo&>);
        static_assert(std::same_as<ForwardT2, Foo&>);
    };

    {
        // template args - deduced
        // passed        - as const value
        const Foo foo;
        using Tuple     = decltype(inputQualifiers(foo, foo))::type;
        using T1        = std::tuple_element_t<0, Tuple>;
        using ForwardT1 = std::tuple_element_t<1, Tuple>;

        using T2        = std::tuple_element_t<2, Tuple>;
        using ForwardT2 = std::tuple_element_t<3, Tuple>;

        static_assert(std::same_as<T1, Foo>);
        static_assert(std::same_as<ForwardT1, Foo&&>);

        static_assert(std::same_as<T2, const Foo&>);
        static_assert(std::same_as<ForwardT2, const Foo&>);
    };

    {
        // template args - T1 implicit, T2 deduced
        // passed        - as const value
        const Foo foo;
        using Tuple     = decltype(inputQualifiers<const Foo&>(foo, foo))::type;
        using T1        = std::tuple_element_t<0, Tuple>;
        using ForwardT1 = std::tuple_element_t<1, Tuple>;

        using T2        = std::tuple_element_t<2, Tuple>;
        using ForwardT2 = std::tuple_element_t<3, Tuple>;

        static_assert(std::same_as<T1, const Foo&>);
        static_assert(std::same_as<ForwardT1, const Foo&>);

        static_assert(std::same_as<T2, const Foo&>);
        static_assert(std::same_as<ForwardT2, const Foo&>);
    };

    {
        // template args - deduced
        // passed        - as const temporary
        const Foo foo;
        using Tuple     = decltype(inputQualifiers(std::move(foo), std::move(foo)))::type;
        using T1        = std::tuple_element_t<0, Tuple>;
        using ForwardT1 = std::tuple_element_t<1, Tuple>;

        using T2        = std::tuple_element_t<2, Tuple>;
        using ForwardT2 = std::tuple_element_t<3, Tuple>;

        static_assert(std::same_as<T1, Foo>);
        static_assert(std::same_as<ForwardT1, Foo&&>);

        static_assert(std::same_as<T2, const Foo>);
        static_assert(std::same_as<ForwardT2, const Foo&&>);
    };

    {
        // template args - T1 implicit, T2 deduced
        // passed        - as const value
        const Foo foo;
        using Tuple     = decltype(inputQualifiers<const Foo&&>(std::move(foo), std::move(foo)))::type;
        using T1        = std::tuple_element_t<0, Tuple>;
        using ForwardT1 = std::tuple_element_t<1, Tuple>;

        using T2        = std::tuple_element_t<2, Tuple>;
        using ForwardT2 = std::tuple_element_t<3, Tuple>;

        static_assert(std::same_as<T1, const Foo&&>);
        static_assert(std::same_as<ForwardT1, const Foo&&>);

        static_assert(std::same_as<T2, const Foo>);
        static_assert(std::same_as<ForwardT2, const Foo&&>);
    };
}

TEST_CASE("deep") {
    /*
    constexpr int value = [] () {
      Foo foo{.value = 5};
      Boo boo{.foo = &foo};

      auto parent = &boo
        | and_then(&Boo::foo)
        | and_then(&Foo::bar)
        | and_then(&Bar::parent)
    }();

    STATIC_REQUIRE(value == 5);
    */
}

struct F {
    int value;
};

template <typename T>
constexpr auto level2(T&& t) {
    static_assert(std::same_as<decltype(t), F&>);

    return std::addressof(t);
}

template <typename T>
constexpr decltype(auto) level1(T&& t) {
    static_assert(std::same_as<decltype(t), F&>);
    // return std::addressof(t) && level2(std::forward<T>(t));
    return level2(std::forward<T>(t));
}

template <typename T>
constexpr decltype(auto) sublevel2(T&& t) {
    static_assert(std::same_as<decltype(t), F&>);
    // return std::addressof(t.value);
    return std::addressof(std::forward<T>(t).value);
}

template <typename T>
constexpr decltype(auto) sublevel1(T&& t) {
    static_assert(std::same_as<decltype(t), F&>);
    // return std::addressof(t.value);
    return sublevel2(std::forward<T>(t));
}

template <typename T>
constexpr decltype(auto) ptr_level2(T&& t) {
    static_assert(std::same_as<decltype(t), F*&&>);
    return std::addressof(*t);
}

template <typename T>
constexpr decltype(auto) ptr_level1(T&& t) {
    static_assert(std::same_as<decltype(t), F*&&>);
    // return std::addressof(*t);
    return ptr_level2(std::forward<T>(t));
}

template <typename T>
constexpr decltype(auto) ptrsub_level2(T&& t) {
    static_assert(std::same_as<decltype(t), F*&&>);

    auto get = [](T&& t) -> int& { return t->value; };

    return std::addressof(get(std::forward<T>(t)));
    // return std::addressof(t->value);
}

template <typename T>
constexpr decltype(auto) ptrsub_level1(T&& t) {
    static_assert(std::same_as<decltype(t), F*&&>);
    // return std::addressof(t->value);
    return ptrsub_level2(std::forward<T>(t));
}

template <typename CPO>
struct pipe2_for {
    inline static constexpr CPO cpo{};

    template <typename Fn>
    struct action {
        Fn fn;

        template <typename Box>
        [[nodiscard]] inline constexpr decltype(auto) operator()(Box&& box) noexcept {
            return cpo(std::forward<Box>(box), std::forward<Fn>(fn));
        }

        template <typename Box, typename A>
            requires(std::same_as<std::remove_cvref_t<A>, action>)
        [[nodiscard]] friend inline constexpr auto operator|(Box&& box, A&& a) noexcept {

            (void)a;
            // return std::addressof(*std::forward<Box>(box));
            return std::forward<A>(a)(std::forward<Box>(box));
        }
    };

    template <typename Fn>
    [[nodiscard]] constexpr decltype(auto) operator()(Fn&& fn) const noexcept {
        return action<decltype(fn)>{std::forward<Fn>(fn)};
    }

    // template <typename Fn>
    // [[nodiscard]] constexpr decltype(auto) operator()() const noexcept {
    // return action<decltype(fn)>{std::forward<Fn>(fn)};
    // }
};

struct op2_fn {
    template <typename Box, typename Fn>
    [[nodiscard]] inline constexpr auto operator()(Box&& box, Fn&& fn) const noexcept {

        auto getValue = [](auto&& box) { return std::forward<Box>(box); };

        return std::forward<Fn>(fn)(getValue(std::forward<Box>(box)));
        // (void) box;
        // (void) fn;
        // using BoxTraits = box_traits_for<Box>;
        // using NewBox = invoke_result_t<
        // decltype(std::forward<Fn>(fn)),
        // decltype(BoxTraits::value(std::forward<Box>(box)))
        // >;
        // using NewBoxTraits = box_traits_for<NewBox>;

        // // if (std::invoke(BoxTraits::has_value, box)) {
        // if (BoxTraits::has_value(box)) {
        // if constexpr (std::is_void_v<typename BoxTraits::value_type>) {
        // return std::forward<Fn>(fn)();
        // } else {
        // return std::forward<Fn>(fn)(BoxTraits::value(std::forward<Box>(box)));
        // }
        // }

        // if constexpr (requires { BoxTraits::error(std::forward<Box>(box)); }) {
        // return NewBoxTraits::lift_error(BoxTraits::error(std::forward<Box>(box)));
        // } else {
        // return NewBoxTraits::lift_error();
        // }
    }
};

inline constexpr pipe2_for<op2_fn> and_then2{};

TEST_CASE("addr") {
    constexpr int v = []() {
        F f{.value = 0};

        if (std::addressof(f) == level1(f)) {
            f.value += 1;
        }

        if (std::addressof(f.value) == sublevel1(f)) {
            f.value += 1;
        }

        if (std::addressof(f) == ptr_level1(&f)) {
            f.value += 1;
        }

        if (std::addressof(f.value) == ptrsub_level1(&f)) {
            f.value += 1;
        }

        if (std::addressof(f) == (&f | and_then2([](auto&& f) { return std::addressof(*f); }))) {
            f.value += 1;
        }

        if (std::addressof(f.value) == (&f | and_then2([](auto&& f) { return std::addressof(f->value); }))) {
            f.value += 1;
        }

        return f.value;
    }();

    STATIC_REQUIRE(v == 6);
}

// TEST_CASE("return-yourself") {
// constexpr int value = [] () {
// Foo foo{.value = 5};

// auto result = &foo
// | and_then([] (auto &f) {
// return &f;
// });
// // | and_then([] (auto &&f) {
// // f.value += 5;
// // return &f;
// // });
// return result->value;
// }();

// STATIC_REQUIRE(value == 10);
// }

/*
 *
 QNetworkRequest request;
request.setUrl(QUrl("http://qt-project.org"));
request.setRawHeader("User-Agent", "MyOwnBrowser 1.0");

manager | and_then(manager::get, request)
        | inspect(setupSignalAndSlots, this)


QNetworkReply *reply = manager->get(request);

auto curl = curl_easy_init();

auto result = curl
  | value_or(CURLUE_BAD_HANDLE)                                                      -> expected<CURL*, CURLcode>
  | and_then(curl_easy_setopt  | as_expected_fn, CURLOPT_URL, "https://example.com") -> expected<CURL*, CURLcode>
  | and_then(curl_easy_perform | as_expected_fn);                                    -> expected<CURL*, CURLcode>
  | finally(curl_easy_cleanup)                                      ->

auto result = curl
  | value_or(CURLUE_BAD_HANDLE)                                     -> expected<CURL*, CURLcode>
  | and_then(curl_easy_setopt, CURLOPT_URL, "https://example.com")  -> expected<CURL*, CURLcode>
  | and_then([] (auto c) {
      auto r = curl_easy_setopt(c, CURLOPT_URL, "https://example.com");
      if (r != CURLE_OK) {
        return unexpected{c, r};
      } else {
        return expected{c};
      }
    }
  | and_then(curl_easy_perform);                                    -> expected<CURL*, CURLcode>
  | finally(curl_easy_cleanup)                                      ->

auto curl = curl_easy_init();

auto result = curl
  | unwarp
  | and_then([] (auto c) { return curl }
  | and_then(curl_easy_perform);

curl_easy_cleanup(curl);

CURL *curl = curl_easy_init();
if(curl) {
  CURLcode res;
  curl_easy_setopt(curl, CURLOPT_URL, "https://example.com");
  res = curl_easy_perform(curl);
  curl_easy_cleanup(curl);
}

auto curl = curl_easy_init();

auto result = curl
  | and_then(curl_easy_setopt, CURLOPT_URL, "https://example.com")
  | and_then(curl_easy_perform)
  | and_then([&curl] {
        char *ct;
        return std::tuple{
          curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct),
          ct,
        }
      };
    });
  | flatten<1>

  | box_as_tuple_with<char *>
  | and_then([&curl] (code, char *ct) {
      return std::tuple{
        curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct),
        ct
      };
    });

curl_easy_cleanup(curl);


  auto curl = curl_easy_init();

  auto result =
      curl
      | and_then(curl_easy_setopt, CURLOPT_URL, "https://www.example.com/")
      | map_to_monad<CURLcode>    // Switch from CURL* to CURLcode
      | and_then(curl_easy_perform)
      | and_then([curl](CURLcode res) -> CURLcode {
            if (res != CURLE_OK) return res;
            char* ct = nullptr;
            auto info_res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
            if (info_res == CURLE_OK && ct) {
                std::printf("We received Content-Type: %s\n", ct);
            }
            return CURLE_OK;
        })
      | finally([curl] { curl_easy_cleanup(curl); });

  if (!has_value(result)) {
      std::fprintf(stderr, "Request failed\n");
      return 1;
  }
*/

} // namespace beman::monadics::tests
