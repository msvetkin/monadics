// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_TESTS_HELPERS_MYEXPECTED_HPP
#define BEMAN_MONADICS_TESTS_HELPERS_MYEXPECTED_HPP

// #define BEMAN_MONADICS_BOX_TRAITS_BUILDER 1
#include <beman/monadics/box_traits.hpp>

#include <utility>
#include <variant>

namespace helpers {

namespace details::_instance_of {

template <typename t, template <typename...> class u>
inline constexpr bool same_as = false;

template <template <typename...> class t, typename... args, template <typename...> class u>
inline constexpr bool same_as<t<args...>, u> = requires { requires std::same_as<t<args...>, u<args...>>; };

} // namespace details::_instance_of

template <typename t, template <typename...> class u>
concept instance_of = requires {
    // requires decomposable<t>;
    requires details::_instance_of::same_as<std::remove_cvref_t<t>, u>;
};

template <typename T, typename E>
    requires(!std::is_void_v<E>)
class [[nodiscard("expected value should be handled")]] expected {
    inline static constexpr bool IsVoid = std::is_void_v<T>;
    using Value                         = std::conditional_t<IsVoid, std::monostate, T>;

  public:
    using value_type = T;
    using error_type = E;

    template <typename Value>
    using rebind = expected<Value, error_type>;

    template <typename Error>
    using rebind_error = expected<value_type, Error>;

    constexpr expected()
        requires std::default_initializable<Value>
        : value_{}, has_value_(true) {}

    template <typename U = value_type>
        requires std::same_as<std::remove_cvref_t<U>, value_type>
    constexpr expected(U&& u)
        requires(!IsVoid)
        : value_(std::forward<U>(u)), has_value_(true) {}

    template <typename U = error_type>
        requires std::same_as<std::remove_cvref_t<U>, error_type>
    constexpr expected(U&& u) : error_(std::forward<U>(u)), has_value_(false) {}

    constexpr expected(const expected&) = default;
    constexpr expected(expected&&)      = default;

    [[nodiscard]] constexpr bool has_value() const noexcept { return has_value_; }

    [[nodiscard]] constexpr decltype(auto) value() & noexcept {
        if constexpr (!IsVoid) {
            return value_;
        }
    }

    [[nodiscard]] constexpr decltype(auto) value() const& noexcept {
        if constexpr (!IsVoid) {
            return value_;
        }
    }

    [[nodiscard]] constexpr decltype(auto) value() && noexcept {
        if constexpr (!IsVoid) {
            return std::move(value_);
        }
    }

    [[nodiscard]] constexpr decltype(auto) value() const&& noexcept {
        if constexpr (!IsVoid) {
            return std::move(value_);
        }
    }

    [[nodiscard]] constexpr decltype(auto) error() & noexcept { return error_; }

    [[nodiscard]] constexpr decltype(auto) error() const& noexcept { return error_; }

    [[nodiscard]] constexpr decltype(auto) error() && noexcept { return std::move(error_); }

    [[nodiscard]] constexpr decltype(auto) error() const&& noexcept { return std::move(error_); }

    [[nodiscard]] friend constexpr bool operator==(const expected& lhs, const expected& rhs) noexcept {
        if (lhs.has_value_ != rhs.has_value_) {
            return false;
        }

        if (lhs.has_value_ != false) {
            if constexpr (!IsVoid) {
                return lhs.value_ == rhs.value_;
            }
        }

        return true;
    }

  private:
    union {
        Value value_;
        E     error_;
    };
    bool has_value_{false};
};

} // namespace helpers

#ifndef BEMAN_MONADICS_BOX_TRAITS_BUILDER
template <typename T, typename E>
struct beman::monadics::box_traits<helpers::expected<T, E>> {
    using value_type = T;
    using error_type = E;

    template <typename T1>
    using rebind_value = helpers::expected<T1, E>;

    template <typename E1>
    using rebind_error = helpers::expected<T, E1>;

    [[nodiscard]] inline static constexpr bool has_value(const auto& box) noexcept { return box.has_value(); }

    [[nodiscard]] inline static constexpr decltype(auto) value(auto&& box) noexcept {
        return std::forward<decltype(box)>(box).value();
    }

    [[nodiscard]] inline static constexpr decltype(auto) error(auto&& box) noexcept {
        return std::forward<decltype(box)>(box).error();
    }

    [[nodiscard]] inline static constexpr helpers::expected<T, E> lift(auto&&... value) noexcept {

        if constexpr (sizeof...(value) == 1) {
            return [](auto&& v) { return std::forward<decltype(v)>(v); }(std::forward<decltype(value)>(value)...);
        } else {
            return {};
        }
    }

    [[nodiscard]] inline static constexpr helpers::expected<T, E> lift_error(auto&& e) noexcept {
        return std::forward<decltype(e)>(e);
    }
};
#else
template <helpers::instance_of<helpers::expected> Box>
struct beman::monadics::box_traits<Box> {
    // [[nodiscard]] inline static constexpr Box lift(auto &&...value) noexcept {

    // if constexpr (sizeof...(value) == 1) {
    // return [] (auto &&v) {
    // return std::forward<decltype(v)>(v);
    // }(std::forward<decltype(value)>(value)...);
    // } else {
    // return {};
    // }

    // }

    // [[nodiscard]] inline static constexpr helpers::expected<T, E> lift_error(auto &&e) noexcept {
    // return std::forward<decltype(e)>(e);
    // }
};
#endif

#endif // BEMAN_MONADICS_TESTS_HELPERS_MYEXPECTED_HPP
