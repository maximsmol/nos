#pragma once

#include <stdint.h>

namespace prtt {
  struct Entry {
    [[gnu::packed]]
    uint64_t loc_seg;
    uint8_t  loc_off;

    [[gnu::packed]]
    uint64_t size_seg;
    uint8_t  size_off;
  };

  struct Table {
    char maginum[4];
    Entry krnl;
    Entry usr;
  };
}
