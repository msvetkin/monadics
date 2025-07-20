// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "beman/monadics/box.hpp"

#include <gtest/gtest.h>

#include <optional>
#include <memory>

namespace beman::monadics::tests {

TEST(Rebind, opt) {
  std::optional box{10};

  using Traits = box_traits<decltype(box)>;

  EXPECT_EQ(Traits::has_value(box), true);
  EXPECT_EQ(Traits::value(box), 10);
}

TEST(Rebind, shared) {
  auto box = std::make_shared<int>(10);

  using Traits = box_traits<decltype(box)>;

  EXPECT_EQ(Traits::has_value(box), true);
  EXPECT_EQ(Traits::value(box), 10);
}

} // namespace beman::monadics::tests

