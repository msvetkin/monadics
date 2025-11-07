// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define BEMAN_MONADICS_BOX_TRAITS_BUILDER 1
#include "beman/monadics/detail/and_then.hpp"

#include <catch2/catch_test_macros.hpp>
#include <memory>

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

template <instance_of<std::shared_ptr> Box>
struct beman::monadics::box_traits<Box> {
    [[nodiscard]] inline static constexpr bool has_value(const Box& box) noexcept { return static_cast<bool>(box); }

    template <typename B = Box>
    [[nodiscard]] inline static constexpr decltype(auto) value(B&& box) noexcept {
        return *std::forward<B>(box);
    }

    [[nodiscard]] inline static constexpr auto error() noexcept { return nullptr; }

    [[nodiscard]] inline static constexpr Box lift(auto&& value) noexcept {
        return std::make_shared<typename Box::element_type>(std::forward<decltype(value)>(value));
    }
};

// #include "helpers/shared-ptr-full.hpp"

namespace beman::monadics::tests {

TEST_CASE("has-box-traits") {
    using Traits = box_traits_for<std::shared_ptr<int>>;
    STATIC_REQUIRE(std::same_as<Traits::value_type, int>);
    STATIC_REQUIRE(std::same_as<Traits::error_type, std::nullptr_t>);
    STATIC_REQUIRE(std::same_as<Traits::rebind_value<double>, std::shared_ptr<double>>);
    STATIC_REQUIRE(std::same_as<Traits::rebind_error<double>, std::shared_ptr<int>>);

    REQUIRE(Traits::lift(10) == std::make_shared<int>(10));
    REQUIRE(Traits::lift_error() == std::shared_ptr<int>{});
    // STATIC_REQUIRE(has_box_traits<std::shared_ptr<int>>);
}

TEST_CASE("with-value") {
    const auto result =
        std::make_shared<int>(10) | and_then([](auto&& value) { return std::make_shared<double>(value * 2.0); });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::shared_ptr<double>>);
    REQUIRE(static_cast<bool>(result));
    REQUIRE(*result == 20.0);
}

TEST_CASE("with-nullptr") {
    const auto result =
        std::shared_ptr<int>{} | and_then([](auto&& value) { return std::make_shared<double>(value * 1.0); });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::shared_ptr<double>>);
    REQUIRE(static_cast<bool>(result) == false);
}

struct Foo {};

struct Boo {
    std::shared_ptr<Foo> foo;
};

TEST_CASE("with-value-return-member") {
    const auto result =
        std::make_shared<Boo>() | and_then([](auto&& boo) { return std::forward<decltype(boo)>(boo).foo; });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::shared_ptr<Foo>>);
    REQUIRE(static_cast<bool>(result) == false);
}

} // namespace beman::monadics::tests
