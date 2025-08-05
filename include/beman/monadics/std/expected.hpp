// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_STD_EXPECTED_HPP
#define BEMAN_MONADICS_STD_EXPECTED_HPP

#include <beman/monadics/concepts/instance_of.hpp>
#include <beman/monadics/box_traits.hpp>

#include <expected>

namespace beman::monadics {

template <instance_of<std::expected> T>
struct box_traits<T> {
    inline static constexpr bool has_value(const auto& box) noexcept { return static_cast<bool>(box); }

    inline static constexpr decltype(auto) value(auto&& box) noexcept {
        return std::forward<decltype(box)>(box).value();
    }

    inline static constexpr decltype(auto) error(auto &&box) noexcept {
        return std::unexpected{std::forward<decltype(box)>(box).error()};
    }
};

} // namespace beman::monadics

#endif // BEMAN_MONADICS_STD_EXPECTED_HPP
