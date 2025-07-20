// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_BOX_HPP
#define BEMAN_MONADICS_BOX_HPP

#include "beman/monadics/rebind.hpp"
#include <concepts> // std::same_as
#include <type_traits>

namespace beman::monadics {

// template <typename T>
// concept box = requires(T t) {
    // // typename T::value_type;
    // { static_cast<bool>(t) } -> std::same_as<bool>;
    // { *t };
// };

template <typename T>
struct box_traits {
  template<typename U>
  inline static constexpr auto False = false;

  static_assert(False<T>, "box_traits has not been implemented for T");
};

template <typename T>
  requires requires (T t){
    { static_cast<bool>(t) } -> std::same_as<bool>;
    { *t };
  }
struct box_traits<T> {
  using box_type = std::remove_cvref_t<T>;

  template<typename U>
  using rebind = rebind<box_type, U>;

  [[nodiscard]] static inline constexpr bool has_value(const box_type& box) noexcept {
    return static_cast<bool>(box);
  }

  template<typename Box>
    requires (std::same_as<std::remove_cvref_t<Box>, box_type>)
  [[nodiscard]] static inline constexpr decltype(auto) value(Box&& box) noexcept {
    return std::forward<decltype(*std::forward<Box>(box))>(*std::forward<Box>(box));
  }
};

}  // namespace beman::monadics

#endif // BEMAN_MONADICS_BOX_HPP
