// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/concepts.hpp>

#include <gtest/gtest.h>

#include <optional>
#include <memory>

namespace beman::monadics::tests {

TEST(ConceptsTest, box) {
  EXPECT_TRUE(Box<std::optional<int>>);
  EXPECT_TRUE(Box<std::unique_ptr<int>>);
  EXPECT_TRUE(Box<std::shared_ptr<int>>);
  EXPECT_FALSE(Box<int>);
}

} // namespace beman::monadics::tests

