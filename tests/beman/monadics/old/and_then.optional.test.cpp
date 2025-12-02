// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "beman/monadics/detail/and_then.hpp"
#include "beman/monadics/box_traits.hpp"
#include "catch2/catch_test_macros.hpp"

#include <catch2/catch_template_test_macros.hpp>

#include <optional>

namespace beman::monadics {

template <typename T>
struct box_type;

template <typename T>
struct box_type<box_traits<T>> {
    using type = T;
};

template <typename T>
using box_type_t = typename box_type<T>::type;

} // namespace beman::monadics

namespace detail::_lift {

template <typename BoxTraits>
struct op_fn {
    [[nodiscard]] friend inline constexpr auto operator|(auto&& value, const op_fn& fn) noexcept {
        using Box   = beman::monadics::box_type_t<BoxTraits>;
        using Value = std::remove_cvref_t<decltype(value)>;

        if constexpr (requires { Box{std::forward<decltype(value)>(value)}; }) {
            return Box{std::forward<decltype(value)>(value)};
        } else {
            return Box{};
        }
    }
};

} // namespace detail::_lift

template <typename BoxTraits>
inline constexpr detail::_lift::op_fn<BoxTraits> lift{};

template <typename T>
struct beman::monadics::box_traits<std::optional<T>> {
    using value_type = T;
    using error_type = std::nullopt_t;

    template <typename V>
    using rebind = std::optional<V>;

    template <typename>
    using rebind_error = std::optional<T>;

    [[nodiscard]] inline static constexpr bool has_value(const auto& box) noexcept { return box.has_value(); }

    [[nodiscard]] inline static constexpr decltype(auto) value(auto&& box) noexcept {
        return std::forward<decltype(box)>(box).value();
    }

    [[nodiscard]] inline static constexpr auto error() noexcept { return std::nullopt; }

    [[nodiscard]] inline static constexpr std::optional<T> lift(auto&& value) noexcept {
        if constexpr (std::same_as<std::remove_cvref_t<decltype(value)>, std::nullopt_t>) {
            return {};
        } else {
            return {std::forward<decltype(value)>(value)};
        }
    }
};

namespace beman::monadics::tests {

TEST_CASE("has-box-traits") { STATIC_REQUIRE(has_box_traits<std::optional<int>>); }

TEST_CASE("myoptional-with-value") {
    constexpr auto result = std::optional{10} | and_then([](auto&& value) {
                                return std::optional{std::forward<decltype(value)>(value) * 2.0};
                            });
    STATIC_REQUIRE(result.has_value());
    STATIC_REQUIRE(result.value() == 20.0);
}

TEST_CASE("myoptional-with-error") {
    constexpr auto result = std::optional<int>{} | and_then([](auto&& value) {
                                return std::optional{std::forward<decltype(value)>(value) * 2.0};
                            });
    STATIC_REQUIRE(result.has_value() == false);
}

TEST_CASE("lift-value") {
    constexpr auto result = 10 | lift<box_traits_for<std::optional<int>>>;
    STATIC_REQUIRE(result.has_value());
}

TEST_CASE("lift-error") {
    constexpr auto result = std::nullopt | lift<box_traits_for<std::optional<int>>>;
    STATIC_REQUIRE(result.has_value() == false);
}

} // namespace beman::monadics::tests
