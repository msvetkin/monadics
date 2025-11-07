// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include "beman/monadics/detail/and_then.hpp"

#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

namespace {

struct Foo {
    int value{};
};
struct Boo {
    Foo* foo{nullptr};
};

} // namespace

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
