// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/type_traits/extract_value_type.hpp>

#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

template <typename T>
struct BoxWithValueType {
  using value_type = T;
};

TEST_CASE("optional") {
    using Value = extract_value_type_t<BoxWithValueType<int>>;
    STATIC_REQUIRE(std::same_as<Value, int>);
}

template <typename T, typename Allocator = void>
struct BoxWithoutValueType {
};

TEST_CASE("custom-type") {
    using Value = extract_value_type_t<BoxWithoutValueType<float>>;
    STATIC_REQUIRE(std::same_as<Value, float>);
}

} // namespace beman::monadics::tests
