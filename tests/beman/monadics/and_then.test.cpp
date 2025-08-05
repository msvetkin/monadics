// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "beman/monadics/and_then.hpp"

#include <catch2/catch_test_macros.hpp>

#include "std.hpp"

namespace beman::monadics::tests {

TEST_CASE("opt-value") {
    using T      = std::optional<int>;
    using traits = box_traits_for<T>;
    constexpr T box{10};

    constexpr auto box2 = box | and_then([](auto v) -> T { return v * 2; });

    STATIC_REQUIRE(traits::has_value(box2));
    STATIC_REQUIRE(traits::value(box2) == 20);
    STATIC_REQUIRE(traits::support_error == false);
}

TEST_CASE("opt-error") {
    using T      = std::optional<int>;
    using traits = box_traits_for<T>;
    constexpr T box{traits::error()};

    constexpr auto box2 = box | and_then([](auto v) -> T { return v * 2; });

    STATIC_REQUIRE(traits::has_value(box2) == false);
    // STATIC_REQUIRE(traits::value(box2) == 20);
    STATIC_REQUIRE(traits::support_error == false);
}


TEST_CASE("shared-ptr-value") {
    struct Foo {
      std::shared_ptr<int> value;
    };

    using T      = std::shared_ptr<Foo>;
    using traits = box_traits_for<T>;
    const T box = traits::lift(Foo{std::make_shared<int>(10)});

    // const auto box2 = box | and_then(foo | lift)
    const auto box2 = box | and_then([](auto &v) {
        return v.value;
        // return lift(v * 2);
        // return (v * 2) | lift;
    });


    //and_then  = lambda -> box
    //transform = lambda -> lift -> box

    REQUIRE(traits::has_value(box2));
    REQUIRE(traits::value(box2) == 10);
    REQUIRE(traits::support_error == false);
}

} // namespace beman::monadics::tests
