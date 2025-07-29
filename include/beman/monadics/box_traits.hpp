// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_TYPE_TRAITS_BOX_HPP
#define BEMAN_MONADICS_TYPE_TRAITS_BOX_HPP

#include <type_traits> // std::remove_cvref_t

namespace beman::monadics {

namespace details::_box_traits {

template <typename T>
concept False = false;

} // namespace details::_box_traits

template <typename T>
struct box_traits {
    static_assert(details::_box_traits::False<T>, "Missing box_traits for the type");
};

namespace details::_box_traits {

template <typename T, typename Traits = box_traits<std::remove_cvref_t<T>>>
struct build : Traits {
    inline static constexpr bool support_error = requires {
        { Traits::error(std::declval<T>()) };
    };
};

} // namespace details::_box_traits

template <typename T>
using box_traits_for = details::_box_traits::build<std::remove_cvref_t<T>>;

} // namespace beman::monadics

#endif // BEMAN_MONADICS_TYPE_TRAITS_BOX_HPP
