// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "beman/monadics/transform.hpp"
#include "beman/monadics/concepts/instance_of.hpp"

#include <catch2/catch_test_macros.hpp>

#include "std.hpp"

namespace beman::monadics::tests {

TEST_CASE("opt-value") {
    using T      = std::optional<int>;
    using traits = box_traits_for<T>;
    constexpr T box{10};

    constexpr auto box2 = box | transform([](auto v) { return v * 3; });

    STATIC_REQUIRE(traits::has_value(box2));
    STATIC_REQUIRE(traits::value(box2) == 30);
    STATIC_REQUIRE(traits::support_error == false);
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

TEST_CASE("opt-result") {
    using T      = Result<void, int>;
    using traits = box_traits_for<T>;

    STATIC_REQUIRE(std::same_as<traits::value_type, void>);
    STATIC_REQUIRE(std::same_as<traits::error_type, int>);

    constexpr T box{};

    constexpr auto box2 = box | transform([]() { return std::string_view{"test"}; });

    STATIC_REQUIRE(traits::has_value(box2));
    STATIC_REQUIRE(traits::value(box2) == "test");
    STATIC_REQUIRE(traits::support_error == true);
}

} // namespace beman::monadics::tests
