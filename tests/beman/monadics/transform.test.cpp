// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/transform.hpp>

#include <gtest/gtest.h>

#include <optional>
#include <type_traits>

namespace beman::monadics::tests {

TEST(TransformTest, opt) {
    const auto r1 = std::optional{10} | transform([](int v) { return v * 2.0; });

    static_assert(std::same_as<std::optional<double>, std::remove_cvref_t<decltype(r1)>>);

    EXPECT_EQ(r1.has_value(), true);
    EXPECT_EQ(r1.value(), 20.0);
}

// } // namespace beman::monadics::tests

// template <typename T>
// struct beman::monadics::box_traits<std::shared_ptr<T>> : beman::monadics::box_traits<typename std::shared_ptr<T>>  {
// using box_type = std::shared_ptr<T>;

// template<typename U>
// using rebind = std::shared_ptr<U>;

// [[nodiscard]] static inline constexpr bool has_value(const box_type& box) noexcept {
// return static_cast<bool>(box);
// }

// template<typename Box>
// requires (std::same_as<std::remove_cvref_t<Box>, box_type>)
// [[nodiscard]] static inline constexpr decltype(auto) value(Box&& box) noexcept {
// return std::forward<decltype(*std::forward<Box>(box))>(*std::forward<Box>(box));
// }
// };

// namespace beman::monadics::tests {

template <template <typename...> class NewBox>
struct lift_to_fn {
    template <typename Box>
    auto operator()(Box&& box) const {
        using Value  = std::remove_cvref_t<Box>::element_type;
        using Traits = box_traits<std::decay_t<Box>>;

        if (!Traits::has_value(box)) {
            return NewBox<Value>{std::nullopt};
        }

        return NewBox<Value>{Traits::value(std::forward<Box>(box))};
    }

    template <typename Box>
    friend inline decltype(auto) operator|(Box&& box, const lift_to_fn& self) {
        return self(std::forward<Box>(box));
    }
};

template <template <typename...> class NewBox>
inline constexpr lift_to_fn<NewBox> lift_to{};

// template <typename T>
// inline constexpr auto lift_to = [] {
// // using Traits = box_traits<std::decay_t<decltype(box)>>;

// // if (!Traits::has_value(box)) {
// // return T{std::nullopt};
// // }

// // return T{Traits::value(std::forward<T>(box))};
// };

TEST(TransformTest, shared) {
    const auto r1 = std::make_shared<int>(10) | lift_to<std::optional> | transform([](int v) { return v * 2.0; });

    // static_assert(std::same_as<
    // std::optional<double>,
    // std::remove_cvref_t<decltype(r1)>
    // >);

    // EXPECT_EQ(r1.has_value(), true);
    // EXPECT_EQ(r1.value(), 20.0);
}

} // namespace beman::monadics::tests
