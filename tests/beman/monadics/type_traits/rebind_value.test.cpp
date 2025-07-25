// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/type_traits/rebind_value.hpp>

#include <catch2/catch_test_macros.hpp>

#include <optional>

namespace beman::monadics::tests {

TEST_CASE("optional") {
    using Opt = rebind_value<std::optional<int>, float>;
    STATIC_REQUIRE(std::same_as<Opt, std::optional<float>>);
}

} // namespace beman::monadics::tests
