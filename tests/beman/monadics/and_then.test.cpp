// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "beman/monadics/and_then.hpp"

#include <catch2/catch_test_macros.hpp>
#include <type_traits>
#include <utility>
#include <variant>

#include "beman/monadics/box_traits.hpp"
#include "beman/monadics/concepts/instance_of.hpp"
#include "std.hpp"

namespace beman::monadics::tests {

TEST_CASE("opt-value") {
    using T      = std::optional<int>;
    using traits = box_traits_for<T>;
    constexpr T box{10};

    constexpr auto box2 = box | and_then([](auto v) -> T { return v * 2; });

    STATIC_REQUIRE(traits::has_value(box2));
    STATIC_REQUIRE(traits::value(box2) == 20);
    STATIC_REQUIRE(traits::support_error == false);
}

TEST_CASE("opt-error") {
    using T      = std::optional<int>;
    using traits = box_traits_for<T>;
    constexpr T box{traits::error()};

    constexpr auto box2 = box | and_then([](auto v) -> T { return v * 2; });

    STATIC_REQUIRE(traits::has_value(box2) == false);
    // STATIC_REQUIRE(traits::value(box2) == 20);
    STATIC_REQUIRE(traits::support_error == false);
}

TEST_CASE("shared-ptr-value") {
    struct Foo {
        std::shared_ptr<int> value;
    };

    using T      = std::shared_ptr<Foo>;
    using traits = box_traits_for<T>;
    const T box  = traits::lift(Foo{std::make_shared<int>(10)});

    // const auto box2 = box | and_then(foo | lift)
    const auto box2 = box | and_then([](auto& v) {
                          return v.value;
                          // return lift(v * 2);
                          // return (v * 2) | lift;
                      });

    // and_then  = lambda -> box
    // transform = lambda -> lift -> box

    REQUIRE(traits::has_value(box2));
    REQUIRE(traits::value(box2) == 10);
    REQUIRE(traits::support_error == false);
}

} // namespace beman::monadics::tests

template <typename T, typename E>
struct Result {
    using Data = std::conditional_t<std::is_void_v<T>, std::variant<std::monostate, E>, std::variant<T, E> >;

    Data data;
};

// template<typename T, typename E>
// struct beman::monadics::box_traits<Result<T, E>> {
template <beman::monadics::instance_of<Result> T>
struct beman::monadics::box_traits<T> {
    inline static constexpr bool has_value(const auto& box) noexcept { return box.data.index() == 0; }

    inline static constexpr decltype(auto) value(auto&& box) noexcept {
        using U = decltype(std::get<0>(std::forward<decltype(box)>(box).data));

        if constexpr (!std::same_as<std::remove_cvref_t<U>, std::monostate>) {
            return std::get<0>(std::forward<decltype(box)>(box).data);
        }
    }

    inline static constexpr decltype(auto) error(auto&& box) noexcept {
        return std::get<1>(std::forward<decltype(box)>(box).data);
    }
};

namespace beman::monadics::tests {

TEST_CASE("result-void") {
    using T      = Result<void, int>;
    using traits = box_traits_for<T>;

    STATIC_REQUIRE(std::same_as<traits::value_type, void>);
    STATIC_REQUIRE(std::same_as<traits::error_type, int>);

    constexpr T box{};
    // STATIC_REQUIRE(traits::error(box) == 10);

    // constexpr Result<std::string_view, int> box2{traits::error(box)};

    constexpr auto box2 = box | and_then([]() { return Result<std::string_view, int>{"msg"}; });

    STATIC_REQUIRE(traits::has_value(box2) == true);
    STATIC_REQUIRE(traits::value(box2) == "msg");
    // STATIC_REQUIRE(traits::support_error == false);
}

} // namespace beman::monadics::tests
