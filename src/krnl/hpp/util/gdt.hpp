#pragma once

#include <stdint.h>
#include "assert.hpp"

namespace util::gdt {
  namespace entry {
    enum class Direction {
      up, down
    };
    struct DataFlags {
      Direction dir;
    };
    struct CodeFlags {
      bool conforming;
    };
    enum class Type {
      data, code
    };
    enum class Size {
      bit16, bit32
    };
  }

  struct Entry {
    uint32_t base;
    uint32_t limit;

    uint8_t ring;
    bool rw;

    entry::Size size;
    entry::Type type;
    union {
      entry::DataFlags data_flags;
      entry::CodeFlags code_flags;
    };

    constexpr Entry(
      uint32_t abase,
      uint32_t alimit,

      uint8_t aring,
      bool arw,

      entry::Size asize,
      entry::Type atype,

      entry::DataFlags adata_flags) :
        base(abase),
        limit(alimit),

        ring(aring),
        rw(arw),

        size(asize),
        type(atype),

        data_flags(adata_flags)
    {}
    constexpr Entry(
      uint32_t abase,
      uint32_t alimit,

      uint8_t aring,
      bool arw,

      entry::Size asize,
      entry::Type atype,

      entry::CodeFlags acode_flags) :
        base(abase),
        limit(alimit),

        ring(aring),
        rw(arw),

        size(asize),
        type(atype),

        code_flags(acode_flags)
    {}

    constexpr uint64_t bin() const {
      constexpr_assert(ring <= 3, "invalid ring in GDT segment");

      bool kib_granularity = false;
      uint32_t limit1 = limit;
      if (limit1 > 0b1111'1111'1111'1111'1111) {
        constexpr_assert((limit1 & (4*1024-1)) == 4*1024-1, "unencodable GDT segment limit");

        limit1 /= 4*1024;
        kib_granularity = true;
      }

      const uint64_t res[8] = {
        limit1 & 0xff,
        (limit1 >> 8) & 0xff,
        base & 0xff,
        (base >> 8) & 0xff,
        (base >> 16) & 0xff,
        static_cast<uint64_t>(1 << 7) +
          static_cast<uint64_t>((ring & 0b11) << 5) +
          (1 << 4) +
          static_cast<uint64_t>((type == entry::Type::data ? 0 : 1) << 3) +
          static_cast<uint64_t>((type == entry::Type::data ? data_flags.dir == entry::Direction::up ? 0 : 1 : code_flags.conforming ? 1 : 0) << 2) +
          static_cast<uint64_t>((rw ? 1 : 0) << 1) +
          0,
        (static_cast<uint64_t>(kib_granularity ? 1 : 0) << 7) +
          (static_cast<uint64_t>(size == entry::Size::bit16 ? 0 : 1) << 6) +
          static_cast<uint64_t>(0 << 5) +
          (0 << 4) +
          ((limit1 >> 16) & 0xf),
        (base >> 24) & 0xff
      };

      return (res[0] & 0xff) +
        ((res[1] & 0xff) << 8) +
        ((res[2] & 0xff) << 16) +
        ((res[3] & 0xff) << 24) +
        ((res[4] & 0xff) << 32) +
        ((res[5] & 0xff) << 40) +
        ((res[6] & 0xff) << 48) +
        ((res[7] & 0xff) << 56);
    }

    constexpr explicit operator uint64_t() const {
      return bin();
    }
  };

  struct GDTR {
    uint16_t gdt_size;
    [[gnu::packed]]
    const uint64_t* gdt;
  };
}
