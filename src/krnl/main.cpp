#include "stdint.hpp"
#include "string.hpp"

#include "term.hpp"
#include "ata.hpp"
#include "ps8042.hpp"
#include "gdt.hpp"
#include "prtt.hpp"

#include "alloc/watermark.hpp"
#include "util/paging.hpp"
#include "scancode_set/2.hpp"

// [[gnu::aligned(0x1000)]] // 4KiB
// static uint32_t pagedir[1024];

void kmain(uint32_t* mem_listing_start, const uint32_t* mem_listing_end, uint8_t* krnl_end) asm("kmain");
void printMemListing(uint32_t* mem_listing_start, const uint32_t* mem_listing_end);
void printprtt(uint8_t* free_mem);

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

  term::init();
  term::clear();
  term::printf("kmain(0x%p, 0x%p, 0x%p)\n", static_cast<void*>(mem_listing_start), static_cast<const void*>(mem_listing_end), static_cast<void*>(krnl_end));
  term::putsln("nos kernel loaded");

  if (static_cast<const void*>(mem_listing_end) < static_cast<const void*>(mem_listing_start))
    term::panicln("Memory map ends before it starts.");
  if (static_cast<const void*>(mem_listing_start) < static_cast<const void*>(krnl_end))
    term::panicln("Memory map starts inside the kernel.");

  uint8_t* free_mem = krnl_end+32;
  alloc::watermark::setup(free_mem, 1024 * 4); // 4 KiB page

  size_t mem_listing_len = static_cast<size_t>(mem_listing_end - mem_listing_start) * sizeof(uint32_t);
  uint32_t* mem_listing_moved = static_cast<uint32_t*>(alloc::watermark::alloc(mem_listing_len));
  memcpy(mem_listing_moved, mem_listing_start, mem_listing_len);

  mem_listing_start = mem_listing_moved;
  mem_listing_end = mem_listing_start + mem_listing_len / sizeof(uint32_t);

  ps8042::Controller c{};
  using namespace scancode_set2;

  uint8_t* print_prtt_mem = static_cast<uint8_t*>(alloc::watermark::alloc(512));

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
          printprtt(print_prtt_mem);
        else if (strcmp(buf, "memlist") == 0)
          printMemListing(mem_listing_start, mem_listing_end);
        else if (strcmp(buf, "clear") == 0)
          term::clear();
        else if (strcmp(buf, "memstat") == 0)
          alloc::watermark::dump_stats();
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

void printMemListing(uint32_t* mem_listing_start, const uint32_t* mem_listing_end) {
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

void printprtt(uint8_t* free_mem) {
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
