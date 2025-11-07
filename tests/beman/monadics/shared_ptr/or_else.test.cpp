// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include "beman/monadics/detail/or_else.hpp"

#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("with-value") {
    const auto result = std::make_shared<int>(10) | or_else([]() { return std::make_shared<int>(1); });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::shared_ptr<int>>);
    REQUIRE(static_cast<bool>(result));
    REQUIRE(*result == 10.0);
}

TEST_CASE("without-value") {
    const auto result = std::shared_ptr<int>{} | or_else([]() { return std::make_shared<int>(1); });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::shared_ptr<int>>);
    REQUIRE(static_cast<bool>(result));
    REQUIRE(*result == 1);
}

} // namespace beman::monadics::tests
