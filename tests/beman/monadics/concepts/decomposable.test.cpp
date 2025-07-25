// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/concepts/decomposable.hpp>

#include <catch2/catch_template_test_macros.hpp>

#include <string>
#include <optional>
#include <tuple>

namespace beman::monadics::tests {

TEMPLATE_TEST_CASE_SIG("decompose",
                       "",
                       ((typename T, bool Expected), T, Expected),
                       (std::optional<int>, true),
                       (std::optional<int>&, true),
                       (std::nullopt_t, false),
                       (std::string, true),
                       ((std::tuple<int, float>), true)) {

    STATIC_REQUIRE(decomposable<T> == Expected);
}

} // namespace beman::monadics::tests
