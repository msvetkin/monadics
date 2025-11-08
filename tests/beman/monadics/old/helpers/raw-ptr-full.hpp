// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_HELPERS_RAW_PTR_FULL_HPP
#define BEMAN_MONADICS_HELPERS_RAW_PTR_FULL_HPP

#include <beman/monadics/box_traits.hpp>

template <typename Box>
    requires std::is_pointer_v<Box>
struct beman::monadics::box_traits<Box> {
    using value_type = std::remove_pointer_t<Box>;

    template <typename V>
    using rebind_value = V*;

    template <typename>
    using rebind_error = Box;

    [[nodiscard]] inline static constexpr bool has_value(const Box box) noexcept { return static_cast<bool>(box); }

    [[nodiscard]] inline static constexpr value_type& value(Box box) noexcept { return *box; }

    [[nodiscard]] inline static constexpr auto error() noexcept { return nullptr; }

    [[nodiscard]] inline static constexpr Box lift(value_type& v) noexcept { return &v; }

    [[nodiscard]] inline static constexpr Box lift_error() noexcept { return nullptr; }
};

#endif // BEMAN_MONADICS_HELPERS_RAW_PTR_FULL_HPP
