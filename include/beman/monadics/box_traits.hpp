// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_TYPE_TRAITS_BOX_HPP
#define BEMAN_MONADICS_TYPE_TRAITS_BOX_HPP

#include "beman/monadics/type_traits/extract_value_type.hpp"
#include "beman/monadics/type_traits/extract_error_type.hpp"
#include "beman/monadics/type_traits/rebind_value.hpp"
#include <beman/monadics/concepts/same_as.hpp>

#include <concepts>    // std::invocable
#include <type_traits> // std::remove_cvref_t
#include <functional>  // std::invoke_result_t

namespace beman::monadics {

namespace details::_box_traits {

template <typename T>
concept False = false;

template <typename Traits>
struct value_type {
    static_assert(False<Traits>, "not able to extract value_type");
};

template <has_value_type Traits>
struct value_type<Traits> {
    using type = typename Traits::value_type;
};

template <typename Traits>
    requires requires {
        requires !has_value_type<Traits>;
        typename extract_value_type_t<typename Traits::box_type>;
        // requires std::same_as<
        // extract_value_type_t<typename Traits::box_type>,
        // std::remove_cvref_t<decltype(Traits::value(std::declval<typename Traits::box_type>()))>
        // >;
    }
struct value_type<Traits> {
    using type = extract_value_type_t<typename Traits::box_type>;
};

template <typename Traits>
using value_type_t = typename value_type<Traits>::type;

template <typename Traits>
struct error_type;

template <has_error_type Traits>
struct error_type<Traits> {
    using type = typename Traits::error_type;
};

template <typename Traits>
    requires requires {
        requires !has_error_type<Traits>;
        requires Traits::support_error;
        typename extract_error_type_t<typename Traits::box_type>;
        requires std::same_as<extract_error_type_t<typename Traits::box_type>,
                              std::remove_cvref_t<decltype(Traits::error(std::declval<typename Traits::box_type>()))>>;
        { typename Traits::box_type{Traits::error(std::declval<typename Traits::box_type>())} };
    }
struct error_type<Traits> {
    using type = extract_error_type_t<typename Traits::box_type>;
};

template <typename Traits>
    requires requires {
        requires !has_error_type<Traits>;
        requires !Traits::support_error;
        requires !std::same_as<void, std::remove_cvref_t<decltype(Traits::error())>>;
        { typename Traits::box_type{Traits::error()} };
    }
struct error_type<Traits> {
    using type = std::remove_cvref_t<decltype(Traits::error())>;
};

template <typename Traits>
using error_type_t = typename error_type<Traits>::type;

template <typename Traits>
struct rebind_value2;

template <typename Traits>
concept has_rebind_value = requires {
    typename Traits::box_type::template rebind_value<typename Traits::value_type>;
    requires std::same_as<typename Traits::box_type,
                          typename Traits::box_type::template rebind_value<typename Traits::value_type>>;
};

template <has_rebind_value Traits>
struct rebind_value2<Traits> {
    template <typename U>
    using type = typename Traits::box_type::template rebind_value<U>;
};

template <typename Traits>
    requires(!has_rebind_value<Traits>)
struct rebind_value2<Traits> {
    template <typename U>
    using type = rebind_value<typename Traits::box_type, U>;
};

template <typename Traits, typename U>
using rebind_value_t = typename rebind_value2<Traits>::template type<U>;

} // namespace details::_box_traits

template <typename T>
struct box_traits {
    static_assert(details::_box_traits::False<T>, "Missing box_traits for the type");
};

namespace details::_box_traits {

template <typename Box, typename Traits = box_traits<Box>>
struct build : Traits {
    using box_type                             = Box;
    inline static constexpr bool support_error = requires {
        { Traits::error(std::declval<Box>()) };
    };

    using value_type = details::_box_traits::value_type_t<build>;
    using error_type = details::_box_traits::error_type_t<build>;

    template <typename U>
    using rebind_value = details::_box_traits::rebind_value_t<build, U>;

    // template<typename U>
    // using rebind_error = rebind_value<Box, U>;
};

} // namespace details::_box_traits

template <typename T>
using box_traits_for = details::_box_traits::build<std::remove_cvref_t<T>>;

} // namespace beman::monadics

#endif // BEMAN_MONADICS_TYPE_TRAITS_BOX_HPP
