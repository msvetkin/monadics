// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_TESTS_STD_HPP
#define BEMAN_MONADICS_TESTS_STD_HPP

#include <beman/monadics/box_traits.hpp>

#include <optional>
#include <memory>

template <typename T>
struct beman::monadics::box_traits<std::optional<T>> {
    inline static constexpr bool has_value(const auto& box) noexcept { return static_cast<bool>(box); }

    inline static constexpr decltype(auto) value(auto&& box) noexcept {
        return std::forward<decltype(box)>(box).value();
    }

    inline static constexpr decltype(auto) error() noexcept { return std::nullopt; }
};

template <typename T>
struct beman::monadics::box_traits<std::shared_ptr<T>> {
    inline static constexpr bool has_value(const auto& box) noexcept { return static_cast<bool>(box); }

    inline static constexpr decltype(auto) value(auto&& box) noexcept { return *std::forward<decltype(box)>(box); }

    inline static constexpr decltype(auto) error() noexcept { return nullptr; }
};

#endif // BEMAN_MONADICS_TESTS_STD_HPP
