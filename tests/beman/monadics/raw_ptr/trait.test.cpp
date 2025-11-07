// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define BEMAN_MONADICS_BOX_TRAITS_BUILDER 1
#include "beman/monadics/box_traits.hpp"

#include <catch2/catch_test_macros.hpp>

#include "trait.hpp"

namespace beman::monadics::tests {

TEST_CASE("box-trait-for") {
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

    constexpr auto liftValue = []() {
        int value = 10;
        return *Traits::lift(value);
    }();
    STATIC_REQUIRE(liftValue == 10);
}

} // namespace beman::monadics::tests
