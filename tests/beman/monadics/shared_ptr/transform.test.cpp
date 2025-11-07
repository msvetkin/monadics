// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include "beman/monadics/detail/transform.hpp"

#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("with-value") {
    const auto result = std::make_shared<int>(10) | transform([](auto&& value) { return value * 2.0; });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::shared_ptr<double>>);
    REQUIRE(static_cast<bool>(result));
    REQUIRE(*result == 20.0);
}

TEST_CASE("with-nullptr") {
    const auto result = std::shared_ptr<int>{} | transform([](auto&& value) { return (value - 1) * 0.5; });
    REQUIRE(std::same_as<decltype(result), const std::shared_ptr<double>>);
    REQUIRE(static_cast<bool>(result) == false);
}

} // namespace beman::monadics::tests
