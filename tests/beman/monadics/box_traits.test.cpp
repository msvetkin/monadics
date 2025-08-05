// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//

#include "beman/monadics/box_traits.hpp"
#include "beman/monadics/concepts/instance_of.hpp"

#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <optional>
#include <memory>

#include "std.hpp"

namespace beman::monadics::tests {

TEST_CASE("opt-value") {
    using T      = std::optional<int>;
    using traits = box_traits_for<T>;
    constexpr T box{10};

    STATIC_REQUIRE(std::same_as<traits::value_type, int>);
    STATIC_REQUIRE(std::same_as<traits::error_type, std::nullopt_t>);
    STATIC_REQUIRE(std::same_as<traits::rebind_value<int>, T>);
    STATIC_REQUIRE(std::same_as<traits::rebind_value<float>, std::optional<float>>);

    STATIC_REQUIRE(traits::has_value(box));
    STATIC_REQUIRE(traits::value(box) == 10);
    STATIC_REQUIRE(traits::support_error == false);
}

TEST_CASE("opt-error") {
    using T      = std::optional<int>;
    using traits = box_traits_for<T>;
    constexpr T box{traits::error()};

    STATIC_REQUIRE(traits::has_value(box) == false);
    STATIC_REQUIRE(traits::support_error == false);
}

TEST_CASE("shared-ptr-value") {
    using T      = std::shared_ptr<int>;
    using traits = box_traits_for<T>;
    const T box{std::make_shared<int>(10)};

    STATIC_REQUIRE(std::same_as<traits::value_type, int>);
    STATIC_REQUIRE(std::same_as<traits::error_type, std::nullptr_t>);
    STATIC_REQUIRE(std::same_as<traits::rebind_value<int>, T>);
    STATIC_REQUIRE(std::same_as<traits::rebind_value<float>, std::shared_ptr<float>>);

    REQUIRE(traits::has_value(box) == true);
    REQUIRE(traits::value(box) == 10);
    REQUIRE(traits::support_error == false);
}

TEST_CASE("shared-ptr-error") {
    using T      = std::shared_ptr<int>;
    using traits = box_traits_for<T>;
    const T box{traits::error()};

    REQUIRE(traits::has_value(box) == false);
    REQUIRE(traits::support_error == false);
}

} // namespace beman::monadics::tests
