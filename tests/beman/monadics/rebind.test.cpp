// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/rebind.hpp>

#include <gtest/gtest.h>

#include <optional>
#include <memory>

namespace beman::monadics::tests {

TEST(Rebind, opt) {
  using Box = std::optional<int>;
  using NewValue = double;
  using NewBox = rebind<Box, NewValue>;
  static_assert(std::same_as<std::optional<double>, NewBox>);
}

TEST(Rebind, shared) {
  using Box = std::shared_ptr<int>;
  using NewValue = double;
  using NewBox = rebind<Box, NewValue>;
  static_assert(std::same_as<std::shared_ptr<double>, NewBox>);
}

template<typename T, typename E>
struct Result {
  using value_type = T;
};

TEST(Rebind, result) {
  using Box = Result<int, int>;
  using NewValue = double;
  using NewBox = rebind<Box, NewValue>;
  static_assert(std::same_as<Result<double, int>, NewBox>);
}

} // namespace beman::monadics::tests

