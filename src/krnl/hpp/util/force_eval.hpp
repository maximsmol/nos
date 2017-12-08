#pragma once

namespace util {
  template <uint64_t V>
  constexpr void force_eval() {
    static_assert(V != V, "compile time force eval");
  }
}
