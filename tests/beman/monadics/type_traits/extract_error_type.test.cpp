// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/type_traits/extract_error_type.hpp>

#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

template <typename E, typename T>
struct BoxWithErrorType {
  using value_type = T;
  using error_type = E;
};

TEST_CASE("optional") {
    using Error = extract_error_type_t<BoxWithErrorType<int, float>>;
    STATIC_REQUIRE(std::same_as<Error, int>);
}

template <typename T, typename E, typename Allocator = void>
struct BoxWithoutErrorType {
};

TEST_CASE("custom-type") {
    using Error = extract_error_type_t<BoxWithoutErrorType<int, float>>;
    STATIC_REQUIRE(std::same_as<Error, float>);
}

} // namespace beman::monadics::tests
