// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define BEMAN_MONADICS_BOX_TRAITS_BUILDER 1
#include "beman/monadics/detail/and_then.hpp"

#include <optional>

#include <catch2/catch_test_macros.hpp>

// #include "helpers/optional-full.hpp"

namespace details::_instance_of {

template <typename t, template <typename...> class u>
inline constexpr bool same_as = false;

template <template <typename...> class t, typename... args, template <typename...> class u>
inline constexpr bool same_as<t<args...>, u> = requires { requires std::same_as<t<args...>, u<args...>>; };

} // namespace details::_instance_of

template <typename t, template <typename...> class u>
concept instance_of = requires {
    // requires decomposable<t>;
    requires details::_instance_of::same_as<std::remove_cvref_t<t>, u>;
};

template <instance_of<std::optional> Box>
struct beman::monadics::box_traits<Box> {
    [[nodiscard]] inline static constexpr auto error() noexcept { return std::nullopt; }
};

namespace beman::monadics::tests {

TEST_CASE("has-box-traits") {
    STATIC_REQUIRE(has_box_traits2<std::optional<int>>);
    using Traits = box_traits_for<std::optional<int>>;
    STATIC_REQUIRE(std::same_as<Traits::value_type, int>);
    STATIC_REQUIRE(std::same_as<Traits::error_type, std::nullopt_t>);
    STATIC_REQUIRE(std::same_as<Traits::rebind<float>, std::optional<float>>);
    STATIC_REQUIRE(std::same_as<Traits::rebind_error<float>, std::optional<int>>);
    STATIC_REQUIRE(Traits::value(std::optional{5}) == 5);
    STATIC_REQUIRE(std::same_as<decltype(Traits::error()), std::nullopt_t>);
    STATIC_REQUIRE(Traits::lift(10) == std::optional{10});
    STATIC_REQUIRE(Traits::lift_error() == std::optional<int>{});
}

TEST_CASE("with-value") {
    constexpr auto result = std::optional{10} | and_then([](auto&& value) {
                                return std::optional{std::forward<decltype(value)>(value) * 2.0};
                            });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::optional<double>>);
    STATIC_REQUIRE(result.has_value());
    STATIC_REQUIRE(result.value() == 20.0);
}

TEST_CASE("with-nullopt") {
    constexpr auto result = std::optional<int>{} | and_then([](auto&& value) {
                                return std::optional{std::forward<decltype(value)>(value) * 2.0};
                            });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::optional<double>>);
    STATIC_REQUIRE(result.has_value() == false);
}

} // namespace beman::monadics::tests
