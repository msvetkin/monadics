// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_STD_OPTIONAL_HPP
#define BEMAN_MONADICS_STD_OPTIONAL_HPP

#include <beman/monadics/concepts/instance_of.hpp>
#include <beman/monadics/box_traits.hpp>

#include <optional>

namespace beman::monadics {

template <instance_of<std::optional> T>
struct box_traits<T> {
    inline static constexpr bool has_value(const auto& box) noexcept { return static_cast<bool>(box); }

    inline static constexpr decltype(auto) value(auto&& box) noexcept {
        return std::forward<decltype(box)>(box).value();
    }

    inline static constexpr decltype(auto) error() noexcept { return std::nullopt; }
};

} // namespace beman::monadics

#endif // BEMAN_MONADICS_STD_OPTIONAL_HPP
