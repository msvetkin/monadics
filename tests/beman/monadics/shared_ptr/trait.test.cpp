// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("box-trait-for") {
    using Traits = box_traits_for<std::shared_ptr<int>>;
    STATIC_REQUIRE(std::same_as<Traits::value_type, int>);
    STATIC_REQUIRE(std::same_as<Traits::error_type, std::nullptr_t>);
    STATIC_REQUIRE(std::same_as<Traits::rebind_value<double>, std::shared_ptr<double>>);
    STATIC_REQUIRE(std::same_as<Traits::rebind_error<double>, std::shared_ptr<int>>);

    const auto sp1 = Traits::lift(10);
    const auto sp2 = std::make_shared<int>(10);
    REQUIRE(sp1);
    REQUIRE(sp2);
    REQUIRE(*sp1 == *sp2);

    REQUIRE(Traits::lift_error() == std::shared_ptr<int>{});
    REQUIRE(Traits::value(std::make_shared<int>(10)) == 10);
}

} // namespace beman::monadics::tests
