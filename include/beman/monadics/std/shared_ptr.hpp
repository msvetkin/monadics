// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_STD_SHARED_PTR_HPP
#define BEMAN_MONADICS_STD_SHARED_PTR_HPP

#include <beman/monadics/concepts/instance_of.hpp>
#include <beman/monadics/box_traits.hpp>

#include <memory>

namespace beman::monadics {

template <instance_of<std::shared_ptr> T>
struct box_traits<T> {
    inline static constexpr bool has_value(const auto& box) noexcept { return static_cast<bool>(box); }

    inline static constexpr decltype(auto) value(auto&& box) noexcept { return *std::forward<decltype(box)>(box); }

    template<typename U>
    inline static constexpr decltype(auto) lift(U&& value) noexcept {
      return std::make_shared<std::remove_cvref_t<U>>(std::forward<U>(value));
    }

    inline static constexpr decltype(auto) error() noexcept { return nullptr; }
};

} // namespace beman::monadics

#endif // BEMAN_MONADICS_STD_SHARED_PTR_HPP
