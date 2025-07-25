// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/concepts/instance_of.hpp>

#include <catch2/catch_template_test_macros.hpp>

#include <string>
#include <optional>
#include <tuple>

namespace beman::monadics::tests {

TEMPLATE_TEST_CASE_SIG("optional",
                       "",
                       ((typename T, bool Expected), T, Expected),
                       (std::optional<int>, true),
                       (std::optional<int>&, true),
                       (std::nullopt_t, false),
                       (std::string, false),
                       ((std::tuple<int, float>), false)) {

    STATIC_REQUIRE(instance_of<T, std::optional> == Expected);
}

TEMPLATE_TEST_CASE_SIG("tuple",
                       "",
                       ((typename T, bool Expected), T, Expected),
                       (std::optional<int>, false),
                       (std::optional<int>&, false),
                       (std::nullopt_t, false),
                       (std::string, false),
                       ((std::tuple<int, float>), true)) {

    STATIC_REQUIRE(instance_of<T, std::tuple> == Expected);
}

} // namespace beman::monadics::tests
