#pragma once

namespace util::internal {
  // generates a call to non-constant function on failure
  void constexpr_assert_fail();
}
#define constexpr_assert(cond, s) \
  ((cond) ? static_cast<void>(0) : util::internal::constexpr_assert_fail())
