#include "alloc/watermark.hpp"

#include "term.hpp"

namespace alloc::watermark {
  static void* alloc_start = nullptr;
  static size_t max_alloc = 0;

  void setup(void* start, size_t max_len) {
    if (alloc_start != nullptr)
      term::panicln("Setup the watermark alloc twice");

    // 4-byte alignment
    alloc_start = reinterpret_cast<void*>((reinterpret_cast<uintptr_t>(start) + 3) & static_cast<uintptr_t>(~3));
    max_alloc = max_len;
  }
  // todo(maximsmol): support other alignment?
  void* alloc(size_t size) {
    char* res = static_cast<char*>(alloc_start);
    size = (size + 3) & static_cast<size_t>(~3);

    if (max_alloc < size)
      term::panicln("Watermark alloc ran out of memory");

    alloc_start = static_cast<void*>(res + size);
    max_alloc -= size;

    #ifndef UNSAFE_ALLOC_NOPOISON
    for (size_t i = 0; i < size; ++i)
      res[i] = static_cast<char>(0xAA); // todo(maximsmol): use memset
    #endif

    return static_cast<void*>(res);
  }

  void dump_stats() {
    term::printf("Watermark allocator:\n  Next alloc at: %p\n  Remaining space: %d\n", alloc_start, max_alloc);
  }
}
