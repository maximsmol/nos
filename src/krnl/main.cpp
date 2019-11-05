#include <stdint.h>

#include "hpp/term.hpp"
#include "hpp/ata.hpp"
#include "hpp/ps8042.hpp"
#include "hpp/gdt.hpp"
#include "hpp/prtt.hpp"

#include "hpp/util/paging.hpp"
#include "hpp/scancode_set/2.hpp"

// [[gnu::aligned(0x1000)]] // 4KiB
// static uint32_t pagedir[1024];

#include "string.h"
int strcmp(const char* l, const char* r) {
  int i = 0;
  while (true) {
    if (l[i] == '\0') {
      if (r[i] == '\0')
        return 0;
      else
        return -1;
    }
    if (r[i] == '\0') {
      if (l[i] == '\0')
        return 0;
      else
        return 1;
    }

    if (l[i] != r[i])
      return l[i] < r[i] ? -1 : 1;
    ++i;
  }
}

int strncmp(const char* l, const char* r, size_t c) {
  size_t i = 0;
  while (true) {
    if (i == c)
      return 0;

    if (l[i] == '\0') {
      if (r[i] == '\0')
        return 0;
      else
        return -1;
    }
    if (r[i] == '\0') {
      if (l[i] == '\0')
        return 0;
      else
        return 1;
    }

    if (l[i] != r[i])
      return l[i] < r[i] ? -1 : 1;
    ++i;
  }
}

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
void printMemListing(uint32_t* mem_listing_start, const uint32_t* mem_listing_end, uint8_t* free_mem);
void printprtt(uint32_t* mem_listing_start, const uint32_t* mem_listing_end, uint8_t* free_mem);

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
  term::clear();
  term::printf("kmain(0x%p, 0x%p, 0x%p)\n", mem_listing_start, mem_listing_end, krnl_end);
  term::putsln("nos kernel loaded");

  ps8042::Controller c{};
  using namespace scancode_set2;

  char buf[25];
  buf[0] = '\0';
  int i = 0;
  term::putch('$');
  while (true) {
    KbdEvent e = pollKbd(c);
    if (e.type == KbdEventType::Release) {
      if (e.code == Scancode::enter) {
        term::nl();

        if (strcmp(buf, "prtt") == 0)
          printprtt(mem_listing_start, mem_listing_end, free_mem);
        else if (strcmp(buf, "memlist") == 0)
          printMemListing(mem_listing_start, mem_listing_end, free_mem);
        else if (strcmp(buf, "clear") == 0)
          term::clear();
        else if (strcmp(buf, "version") == 0) {
          term::puts("       _____ _____ \n"
                     "      |  _  /  ___|\n"
                     " _ __ | | | \\ `--. \n"
                     "| '_ \\| | | |`--. \\\n"
                     "| | | \\ \\_/ /\\__/ /\n"
                     "|_| |_|\\___/\\____/ \n"
                     "                   \n"
                     "                   \n");
          term::putsln("Running nOS v0.1.0");
        }
        else if (strncmp(buf, "echo ", 5) == 0) {
          int x = 5;
          while (buf[x] != '\0') {
            term::putch(buf[x]);
            ++x;
          }
          term::nl();
        }
        else {
          term::printf("kconsole: %s: command not found\n", buf);
        }

        buf[0] = '\0'; i =0;
      }
      else if (e.code == Scancode::backspace) {
        if (i > 0) {
          --i;
          buf[i] = '\0';
        }
      }
      else {
        char x = scancode_set2::scancodeToChar(e.code);
        if (x != '\0') {
          buf[i++] = x;
          buf[i] = '\0';
        }
      }
    }
    term::clearLine();
    term::printf("$ %s", buf);
    // term::printf("ps8042-1: %s, %s\n", scancode_set2::scancodeName(e.code), e.type == KbdEventType::Press ? "press" : "release");
  }
}

void printMemListing(uint32_t* mem_listing_start, const uint32_t* mem_listing_end, uint8_t* free_mem) {
  term::putsln("BIOS memory map:");
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
}

void printprtt(uint32_t* mem_listing_start, const uint32_t* mem_listing_end, uint8_t* free_mem) {
  ata::Bus ata0(ata::stdData::bus::ioport::primary);
  ata::Drive dr = *ata0.master().lookforDrive();

  dr.writeSecCount(1);
  dr.writeSec(3);
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

  term::putsln("Read partition table:");
  for (unsigned int i = 0; i < 512/sizeof(uint16_t); ++i) {
    uint16_t chunk = dr.readData();
    memcpy(free_mem + i*sizeof(uint16_t), &chunk, sizeof(uint16_t));
  }

  prtt::Table prtt{};
  memcpy(&prtt, free_mem, sizeof(prtt::Table));
  prtt::Entry krnl_prt = prtt.krnl;
  prtt::Entry usr_prt = prtt.usr;

  term::printf("%c%c%c%c:\n", prtt.maginum[0],prtt.maginum[1],prtt.maginum[2],prtt.maginum[3]);
  term::printf("  krnl: @%lld:%hd size = %lld:%hd = %lld\n", krnl_prt.loc_seg, krnl_prt.loc_off, krnl_prt.size_seg, krnl_prt.size_off, (krnl_prt.size_seg<<9) + krnl_prt.size_off);
  term::printf("  usr: @%lld:%hd size = %lld:%hd = %lld\n", usr_prt.loc_seg, usr_prt.loc_off, usr_prt.size_seg, usr_prt.size_off, (usr_prt.size_seg<<9) + usr_prt.size_off);
}
