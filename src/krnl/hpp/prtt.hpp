#pragma once

#include <stdint.h>

namespace prtt {
  struct Entry {
    uint64_t loc_seg;
    uint16_t  loc_off;

    uint64_t size_seg;
    uint16_t  size_off;
  };

  struct Table {
    char maginum[4];
    Entry krnl;
    Entry usr;
  };
}
