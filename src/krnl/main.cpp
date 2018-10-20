#include <stdint.h>

#include "hpp/term.hpp"
#include "hpp/ata.hpp"
#include "hpp/gdt.hpp"
#include "hpp/prtt.hpp"

#include "hpp/util/paging.hpp"

// [[gnu::aligned(0x1000)]] // 4KiB
// static uint32_t pagedir[1024];

#include "string.h"
void* memcpy(void* dist, const void* src, size_t len) {
  char* d = static_cast<char*>(dist);
  const char* s = static_cast<const char*>(src);

  for (size_t i = 0; i < len; ++i)
    *(d+i) = *(s+i);

  return dist;
}
void* memset(void* dist, int ch, size_t len) {
  unsigned char* d = static_cast<unsigned char*>(dist);
  unsigned char fill = static_cast<unsigned char>(ch);

  for (size_t i = 0; i < len; ++i)
    *(d+i) = fill;

  return dist;
}

void kmain(uint32_t* mem_listing_start, const uint32_t* mem_listing_end, uint8_t* krnl_end) asm("kmain");

void kmain(uint32_t* mem_listing_start, const uint32_t* mem_listing_end, uint8_t* krnl_end) {
  asm volatile(
    "lgdt %[gdtr]\n"
    "jmp %[selector_code], $1f\n"
    "1:\n"
    "mov %[selector_data], %%ds\n"
    "mov %[selector_data], %%es\n"
    "mov %[selector_data], %%ss\n"
    "mov %[selector_data], %%fs\n"
    "mov %[selector_data], %%gs"
    :
    : [gdtr]"m"(gdt::gdtr), [selector_code]"i"(gdt::selector::code), [selector_data]"r"(gdt::selector::data));

  uint8_t* free_mem = krnl_end+1;

  term::init();
  term::printf("kmain(0x%p, 0x%p, 0x%p)\n", mem_listing_start, mem_listing_end, krnl_end);
  // return;

  term::putsln("nos");

  ata::Bus ata0(ata::stdData::bus::ioport::primary);
  ata::Drive dr = *ata0.master().lookforDrive();

  dr.writeSecCount(1);
  dr.writeSec(2);
  dr.writeCylLo(0);
  dr.writeCylHi(0);
  dr.writeHead(0);
  dr.writeCommand(ata::stdData::command::read);

  ata::DriveStatus s(dr.readStatus());
  while (!s.drq()) {
    s = dr.readStatus();
    if (s.df()) {
      term::puts("drive failure");
      return;
    }
    if (s.err()) {
      term::puts("drive error");
      return;
    }
  }

  term::putsln("read");
  for (unsigned int i = 0; i < 512/sizeof(uint16_t); ++i) {
    uint16_t chunk = dr.readData();
    memcpy(free_mem + i*sizeof(uint16_t), &chunk, sizeof(uint16_t));
  }

  prtt::Table prtt{};
  memcpy(&prtt, free_mem, sizeof(prtt::Table));
  prtt::Entry krnl_prt = prtt.krnl;
  prtt::Entry usr_prt = prtt.usr;

  int i = 0;
  while (mem_listing_start < mem_listing_end) {
    if (i >= 25)
      break;

    uint64_t base = 0;
    memcpy(&base, mem_listing_start, sizeof(uint64_t));
    mem_listing_start++; mem_listing_start++;

    uint64_t len = 0;
    memcpy(&len, mem_listing_start, sizeof(uint64_t));
    mem_listing_start++; mem_listing_start++;

    uint32_t type = *mem_listing_start++;
    uint32_t eattr = *mem_listing_start++;

    term::printf("%d: %llx -> %llx ", i, base&0xffffffff/*(base&0xffffffff00000000)>>32*/, base+len);
    if (type == 1)
      term::putsln("ok");
    else if (type == 2)
      term::putsln("res");
    else if (type == 3)
      term::putsln("rec");
    else if (type == 4)
      term::putsln("nvs");
    else if (type == 5)
      term::putsln("bad");
    else
      term::printf("? %d\n", type);

    ++i;
  }

  // dr.writeSecCount(0);
  // dr.writeSec(0);
  // dr.writeCylLo(0);
  // dr.writeCylHi(0);
  // dr.writeCommand(ata::stdData::command::identify);

  // ata::DriveStatus s(dr.readStatus());
  // term::printf("f:%d e:%d d:%d", s.raw() == 0, s.err(), s.drq());
}
