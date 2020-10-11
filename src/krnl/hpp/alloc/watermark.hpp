#pragma once

#include "stdint.hpp"

namespace alloc::watermark {
  void setup(void* start, size_t max_len);
  void* alloc(size_t size) __attribute__((alloc_size(1)));
  void dump_stats();
}
