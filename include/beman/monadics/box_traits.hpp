// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_BOX_TRAITS_HPP
#define BEMAN_MONADICS_BOX_TRAITS_HPP

#include <beman/monadics/detail/extract_value_type.hpp>

#include <concepts>
#include <type_traits>

namespace beman::monadics {

template <typename T>
struct box_traits;

namespace detail::_box_traits {

template <typename T>
concept has_specialization = requires {
    typename box_traits<T>;
    { box_traits<T>{} };
};

template <typename T>
concept deducable_value_type = requires { typename extract_value_type_t<T>; };

template <typename T, typename Traits>
concept has_error_fn = requires {
    { Traits::error(std::declval<T>()) } noexcept -> std::convertible_to<typename Traits::error_type>;
} || requires {
    { Traits::error() } noexcept -> std::convertible_to<typename Traits::error_type>;
};

template <typename T>
concept has_minimal_traits = requires {
    requires detail::_box_traits::has_specialization<T>;
    { box_traits<T>::has_value(std::declval<T>()) } noexcept -> std::same_as<bool>;
    { box_traits<T>::value(std::declval<T>()) } noexcept -> std::convertible_to<typename box_traits<T>::value_type>;
    requires has_error_fn<T, box_traits<T>>;
};

template <typename T, typename Traits = box_traits<T>>
concept has_full_traits = requires {
    requires has_minimal_traits<T>;
    typename Traits::value_type;
    typename Traits::template rebind_value<typename Traits::value_type>;
    typename Traits::error_type;
    typename Traits::template rebind_error<typename Traits::error_type>;
};

template <typename T, typename Traits = box_traits<T>>
concept has_deducable_traits = requires {
    requires has_minimal_traits<T>;
    // requires typename Traits::value_type;
    // requires typename Traits::template rebind_value<typename Traits::value_type>;
    // requires typename Traits::error_type;
    // requires typename Traits::template rebind_error<typename Traits::error_type>;
};

} // namespace detail::_box_traits

template <typename T>
concept has_box_traits = requires {
    requires detail::_box_traits::has_minimal_traits<std::remove_cvref_t<T>>;
    requires(detail::_box_traits::has_full_traits<std::remove_cvref_t<T>> ||
             detail::_box_traits::has_deducable_traits<std::remove_cvref_t<T>>);
};

template <has_box_traits T>
using box_traits_for = box_traits<std::remove_cvref_t<T>>;

/*

has_specialization<T>
  box_traits<T>
  has_minimal_box_traits<T>
    box_traits<T>::has_value();
    box_traits<T>::value();
    box_traits<T>::error();
  has_full_box_traits<T>
    box_traits<T>::value_type
    box_traits<T>::error_type
    box_traits<T>::rebind_value
    box_traits<T>::rebind_error
    box_traits<T>::has_value();
    box_traits<T>::value();
    box_traits<T>::error();

  box_traits<T>;
  box_traits<T>::value_type   || deducable_value_type<T>;
  box_traits<T>::error_type   || deducable_error_type<T>;
  box_traits<T>::rebind_value || deducable_rebind_value<T>;
  box_traits<T>::rebind_error || deducable_rebind_error<T>;

template<typename T>
struct box_traits_for;

template<typename T>
  requires requires {
    has_specialization<T>;
    has_minimal_traits<T>;
    has_full_traits<T>;
  }
struct box_traits_for {
  using traits_type = box_traits<T>;
};

template<has_minimal_traits T>
  requires requires {
    has_specialization
    has_minimal_traits
    has_deducable_traits
  }
struct box_traits_for {
 private:
  using base_traits = box_traits<T>;

 public:
   struct traits_type : base_traits {
      usinng value_type = get_value_type_or_deduce<T, base_traits>
      usinng error_type = get_error_type_or_deduce<T, base_traits>

      template<typename V>
      using rebind_value = get_rebind_error_or_deduce<V, base_traits>

      template<typename E>
      usinng error_type = get_rebind_error_or_deduce<E, base_traits>
   };
};


*/

} // namespace beman::monadics

#endif // BEMAN_MONADICS_BOX_TRAITS_HPP
