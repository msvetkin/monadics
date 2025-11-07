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

    constexpr auto value = []() {
        int value = 10;
        return Traits::value(&value);
    }();
    STATIC_REQUIRE(value == 10);

    constexpr auto hasValue = []() {
        int value = 10;
        return Traits::has_value(&value);
    }();
    STATIC_REQUIRE(hasValue);

    constexpr auto noValue = []() { return Traits::has_value(static_cast<int*>(nullptr)); }();
    STATIC_REQUIRE(noValue == false);

    STATIC_REQUIRE(Traits::error() == nullptr);
    STATIC_REQUIRE(Traits::lift_error() == static_cast<int*>(nullptr));

    // unsupported
    // STATIC_REQUIRE(Traits::lift(10));
}

TEST_CASE("with-value") {
    constexpr int value = []() {
        Foo foo{.value = 5};
        Boo boo{.foo = &foo};

        auto result = &boo | and_then([&](auto&& b) { return b.foo; }) | and_then([](Foo& f) {
            f.value += 5;
            return &f;
        });

        return result->value;
    }();

    STATIC_REQUIRE(value == 10);
}

TEST_CASE("with-null") {
    constexpr int value = []() {
        int* p{nullptr};

        auto result = p | and_then([&](auto&& v) { return &v; });

        return result ? 10 : -10;
    }();

    STATIC_REQUIRE(value == -10);
}

TEST_CASE("self") {
    constexpr int value = []() {
        Foo foo{.value = 5};
        Boo boo{.foo = &foo};

        auto result = &boo | and_then([](auto&& b) { return &b; }) | and_then([](auto&& b) { return &b; });
        return result->foo->value;
    }();

    STATIC_REQUIRE(value == 5);
}

} // namespace beman::monadics::tests
