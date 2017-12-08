#pragma once

#include <stdint.h>
#include "assert.hpp"

namespace util::paging {
  namespace dir {
    struct Entry {
      uint32_t addr;

      bool extended;
      bool cache;
      bool wthrough;
      bool usr;
      bool rw;
      bool present;

      uint8_t etc;

      constexpr uint32_t bin() const {
        constexpr_assert(!extended, "extended pages are not supported");

        uint32_t addr1 = addr & 0xfffff000;
        constexpr_assert(addr1 == addr, "page table addr must be [align 4KiB]");
        addr1 >>= 12;

        constexpr_assert((etc & ~0xf) == 0, "etc has to be <= 4b");

        const uint32_t res[4] = {
          static_cast<uint32_t>((extended ? 1 : 0) << 7) +
            (0 << 6) +
            (0 << 5) +
            static_cast<uint32_t>((cache ? 0 : 1) << 4) +
            static_cast<uint32_t>((wthrough ? 1 : 0) << 3) +
            static_cast<uint32_t>((usr ? 1 : 0) << 2) +
            static_cast<uint32_t>((rw ? 1 : 0) << 1) +
            (present ? 1 : 0),
          (etc & 0xf) +
            ((addr1 & 0xf) << 4),
          (addr1 >> 4 ) & 0xff,
          (addr1 >> 12) & 0xff
        };

        return (res[0] & 0xff) +
          ((res[1] & 0xff) << 8) +
          ((res[2] & 0xff) << 16) +
          ((res[3] & 0xff) << 24);
      }

      constexpr explicit operator uint32_t() const {
        return bin();
      }
    };
  }
  namespace table {
    struct Entry {
      uint32_t addr;

      bool global;
      bool cache;
      bool wthrough;
      bool usr;
      bool rw;
      bool present;

      uint8_t etc;

      constexpr uint32_t bin() const {
        uint32_t addr1 = addr & 0xfffff000;
        constexpr_assert(addr1 == addr, "page addr must be [align 4KiB]");
        addr1 >>= 12;

        constexpr_assert((etc & ~0b111) == 0, "etc has to be <= 3b");

        const uint32_t res[4] = {
          static_cast<uint32_t>(0 << 7) +
            (0 << 6) +
            (0 << 5) +
            static_cast<uint32_t>((cache ? 0 : 1) << 4) +
            static_cast<uint32_t>((wthrough ? 1 : 0) << 3) +
            static_cast<uint32_t>((usr ? 1 : 0) << 2) +
            static_cast<uint32_t>((rw ? 1 : 0) << 1) +
            (present ? 1 : 0),
          static_cast<uint32_t>(global ? 1 : 0) +
            static_cast<uint32_t>((etc & 0b111) << 1) +
            ((addr1 & 0b1111) << 4),
          (addr1 >> 4 ) & 0xff,
          (addr1 >> 12) & 0xff
        };

        return (res[0] & 0xff) +
          ((res[1] & 0xff) << 8) +
          ((res[2] & 0xff) << 16) +
          ((res[3] & 0xff) << 24);
      }

      constexpr explicit operator uint32_t() const {
        return bin();
      }
    };
  }
}
