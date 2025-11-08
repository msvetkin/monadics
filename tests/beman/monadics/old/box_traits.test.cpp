// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/box_traits.hpp>

#include <catch2/catch_template_test_macros.hpp>

namespace monadics = beman::monadics;

struct Foo {
    int  value;
    bool has_value{false};
};

template <>
struct monadics::box_traits<Foo> {
    using value_type = int;
    using error_type = double;

    template <typename>
    using rebind_value = Foo;

    template <typename>
    using rebind_error = Foo;

    [[nodiscard]] inline static constexpr bool has_value(const auto& foo) noexcept { return foo.has_value; }

    [[nodiscard]] inline static constexpr decltype(auto) value(auto&& foo) noexcept {
        return std::forward<decltype(foo)>(foo).value;
    }

    [[nodiscard]] inline static constexpr double error() noexcept { return 55.5; }
};

TEMPLATE_TEST_CASE_SIG("has-box-traits", "", ((typename T, bool Expected), T, Expected), (Foo, true), (int, false)) {
    STATIC_REQUIRE(monadics::has_box_traits<T> == Expected);
}
