// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define BEMAN_MONADICS_BOX_TRAITS_BUILDER 1
#include "beman/monadics/detail/or_else.hpp"

#include <catch2/catch_test_macros.hpp>

#include "helpers/raw-ptr-full.hpp"

namespace beman::monadics::tests {

TEST_CASE("has-box-traits") {
    // STATIC_REQUIRE(has_box_traits<int*>);
}

TEST_CASE("with-value") {
    constexpr int value = []() {
        int  value{10};
        int* p = nullptr;

        auto result = p | or_else([&value]() { return &value; });
        return *result;
    }();

    STATIC_REQUIRE(value == 10);
}

} // namespace beman::monadics::tests
