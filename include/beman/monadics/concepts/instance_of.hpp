// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_CONCEPTS_INSTANCE_OF_HPP
#define BEMAN_MONADICS_CONCEPTS_INSTANCE_OF_HPP

#include <beman/monadics/concepts/decomposable.hpp>

namespace beman::monadics {

template <typename T, template <typename...> class U>
concept instance_of = requires {
    requires decomposable<T>;
    []<typename... Args>(U<Args...>*) {}(static_cast<std::remove_cvref_t<T>*>(nullptr));
};

} // namespace beman::monadics

#endif // BEMAN_MONADICS_CONCEPTS_INSTANCE_OF_HPP
