// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "beman/monadics/detail/transform.hpp"
#include "beman/monadics/box_traits.hpp"

#include "helpers/optional-full.hpp"
#include "helpers/shared-ptr-full.hpp"

#include "catch2/catch_test_macros.hpp"

#include <catch2/catch_template_test_macros.hpp>

#include <concepts>
#include <cstddef>
#include <memory>
#include <optional>
#include <source_location>
#include <utility>
#include <iostream>

template <typename T>
class smart_ptr {
  public:
    constexpr smart_ptr() = default;

    constexpr smart_ptr(const smart_ptr<T>& other) : value_(std::exchange(other.value_, nullptr)) {}

    constexpr smart_ptr(smart_ptr<T>&& other) : value_(std::exchange(other.value_, nullptr)) {}

    template <std::convertible_to<T> U>
    constexpr smart_ptr(U&& u) : value_(new T{std::forward<U>(u)}) {}

    constexpr ~smart_ptr() { delete value_; }

    [[nodiscard]] constexpr bool has_value() const noexcept { return value_ != nullptr; }

    [[nodiscard]] constexpr T& value() noexcept { return *value_; }

    [[nodiscard]] constexpr T& value() const noexcept { return *value_; }

  private:
    T* value_{nullptr};
};

template <typename T>
struct beman::monadics::box_traits<smart_ptr<T>> {
    using value_type = T;
    using error_type = std::nullopt_t;

    template <typename V>
    using rebind = smart_ptr<V>;

    template <typename>
    using rebind_error = smart_ptr<T>;

    [[nodiscard]] inline static constexpr bool has_value(const auto& box) noexcept { return box.has_value(); }

    [[nodiscard]] inline static constexpr decltype(auto) value(auto&& box) noexcept {
        return std::forward<decltype(box)>(box).value();
    }

    [[nodiscard]] inline static constexpr auto error() noexcept { return std::nullopt; }

    [[nodiscard]] inline static constexpr smart_ptr<T> lift(auto&& value) noexcept {
        if constexpr (std::same_as<std::remove_cvref_t<decltype(value)>, std::nullopt_t>) {
            return {};
        } else {
            return {std::forward<decltype(value)>(value)};
        }
    }
};

namespace beman::monadics::tests {

TEST_CASE("has-box-traits") { STATIC_REQUIRE(has_box_traits<std::optional<int>>); }

TEST_CASE("with-value") {
    constexpr auto result = std::optional{10} | transform([](auto&& value) { return value * 2.0; });

    STATIC_REQUIRE(result.has_value());
    STATIC_REQUIRE(result.value() == 20.0);
}

TEST_CASE("without-value") {
    constexpr auto result = std::optional<int>{} | transform([](auto&& value) { return value * 2.0; });
    STATIC_REQUIRE(result.has_value() == false);
}

TEST_CASE("shared-ptr-with-value") {
    auto ptr = std::make_shared<int>(10);

    const auto result = ptr | transform([](auto&& value) { return value * 2.0; });

    REQUIRE(result);
    REQUIRE(*result == 20.0);
}

TEST_CASE("shared-ptr-without-value") {
    std::shared_ptr<int> ptr;

    const auto result = ptr | transform([](auto&& value) { return value * 2.0; });

    REQUIRE(static_cast<bool>(result) == false);
}

TEST_CASE("shared-ptr-rvalue-value") {

    const auto result = std::make_shared<int>(10) | transform([](auto&& value) { return value * 2.0; });

    REQUIRE(static_cast<bool>(result) == true);
}

struct Item {
    std::string name;
    virtual ~Item() {};
};

struct Widget : Item {};

struct Some {
    std::shared_ptr<Item> parent;
};

TEST_CASE("shared-ptr-parent-child") {
    const auto parent = [] {
        auto some          = std::make_shared<Some>();
        some->parent       = std::make_shared<Item>();
        some->parent->name = "test";

        return std::move(some) | and_then([](auto&& some) { return some.parent; });
    }();

    REQUIRE(parent);
    REQUIRE(parent->name == "test");
}

TEST_CASE("smart-ptr-rvalue-value") {
    constexpr auto result = []() {
        smart_ptr<int> ptr{10};
        const auto     value = ptr.value();
        return value;
    }();

    STATIC_REQUIRE(result == 10);
    // constexpr smart_ptr<int> ptr{10};

    // constexpr auto result2 = [] () {
    // smart_ptr<int> ptr{10};
    // return ptr | transform([] (auto &&value) {
    // return value *2.0;
    // });
    // }();

    // STATIC_REQUIRE(result2.has_value());
}

struct noisy {
    inline static std::size_t ctor{0};
    inline static std::size_t copy_ctor{0};
    inline static std::size_t copy_assigment{0};
    inline static std::size_t move_ctor{0};
    inline static std::size_t move_assigment{0};
    inline static std::size_t dtor{0};

    static void reset() noexcept {
        ctor           = 0;
        copy_ctor      = 0;
        copy_assigment = 0;
        move_ctor      = 0;
        move_assigment = 0;
        dtor           = 0;
    }

    noisy(std::source_location loc = std::source_location::current()) : loc_(loc) {
        std::cout << "ctor at: " << loc.file_name() << ":" << loc.line() << " " << loc.function_name() << "\n";
        ctor++;
    }

    noisy(const noisy&, std::source_location loc = std::source_location::current()) : loc_(loc) {
        std::cout << "copy_ctor at: " << loc.file_name() << ":" << loc.line() << " " << loc.function_name() << "\n";
        copy_ctor++;
    }

    noisy& operator=(const noisy&) {
        copy_assigment++;
        return *this;
    }

    noisy(noisy&&, std::source_location loc = std::source_location::current()) : loc_(loc) {
        std::cout << "move_ctor at: " << loc.file_name() << ":" << loc.line() << " " << loc.function_name() << "\n";
        move_ctor++;
    }

    noisy& operator=(noisy&&) {
        move_assigment++;
        return *this;
    }

    ~noisy() {
        std::cout << "dtor at: " << loc_.file_name() << ":" << loc_.line() << " " << loc_.function_name() << "\n";
        dtor++;
    }

  private:
    std::source_location loc_;
};

template <typename T>
void dump() {
    std::cout << "  " << std::source_location::current().function_name() << "\n";
}

TEST_CASE("noisy") {
    noisy::reset();

    auto value = [](auto&& n) {
        std::cout << "inside value fn\n";
        dump<decltype(std::forward<decltype(n)>(n))>();
        return n.ctor;
    };

    auto tr = [&value](auto&& n) {
        std::cout << "inside tr fn, start\n";
        auto v = value(std::forward<decltype(n)>(n)) * 2.0;
        std::cout << "inside tr fn, end\n";
        return v;
    };

    const auto result = tr(noisy{});
    // noisy n{};
    // noisy n1 = n;
    // noisy{}
    REQUIRE(noisy::ctor == 1);
    REQUIRE(noisy::copy_ctor == 0);
    REQUIRE(noisy::move_ctor == 0);
    REQUIRE(noisy::copy_assigment == 0);
    REQUIRE(noisy::move_assigment == 0);
    REQUIRE(noisy::dtor == 1);
}

template <typename Box, typename BoxTraits>
struct get_value {
    using fn = typename BoxTraits::value_type& (Box::*)() &;
};

template <typename Box, typename BoxTraits>
struct get_value<Box&, BoxTraits> {
    using fn = typename BoxTraits::value_type& (Box::*)() &;
};

template <typename Box, typename BoxTraits>
struct get_value<const Box&, BoxTraits> {
    using fn = const typename BoxTraits::value_type& (Box::*)() const&;
};

TEST_CASE("as-box") {
    constexpr auto result = [] {
        std::optional<int> op{10};

        using T     = std::optional<int>;
        using fn    = typename T::value_type& (T::*)() &;
        fn value_fn = &decltype(op)::value;

        int& (T::*get_value)() & = &T::value;

        return std::invoke(get_value, op);
    }();

    STATIC_REQUIRE(result == 10);

    using T                                                   = int;
    using Box                                                 = std::optional<T>;
    constexpr T& (Box::*value_ref)() &                        = &Box::value;
    constexpr T && (Box::*value_rvalue)() &&                  = &Box::value;
    constexpr const T& (Box::*value_const_ref)() const&       = &Box::value;
    constexpr const T && (Box::*value_const_rvalue)() const&& = &Box::value;

    constexpr get_value<Box&, box_traits_for<Box>>::fn value_ref2 = &Box::value;
    STATIC_REQUIRE(value_ref == value_ref2);

    constexpr get_value<const Box&, box_traits_for<Box>>::fn value_const_ref2 = &Box::value;
    STATIC_REQUIRE(value_const_ref == value_const_ref2);
    // T &&(Box::*value_rvalue)()    && = &Box::value;

    STATIC_REQUIRE(static_cast<T& (Box::*)() &>(&Box::value) == value_ref);
    // template<typename Box>
    // inline static constexpr T&(Box::*value)() & = &Box::value;
}

} // namespace beman::monadics::tests
