// #include "hpp/cpuio.hpp"

#pragma once

#include <stdint.h>

namespace cpuio {
  using port_t = uint16_t;

  // N  for `out imm8, eax/ax/al`
  // dx for `out dx, eax/ax/al`
  // note: port always in dx, value always in eax/ax/al

  [[gnu::always_inline]]
  inline void outb(const port_t p, const uint8_t v) {
    asm volatile("outb %[value], %[port]" : : [port]"Nd"(p), [value]"a"(v));
  }

  [[gnu::always_inline]]
  inline void outw(const port_t p, const uint16_t v) {
    asm volatile("outw %[value], %[port]" : : [port]"Nd"(p), [value]"a"(v));
  }

  [[gnu::always_inline]]
  inline void outd(const port_t p, const uint32_t v) {
    asm volatile("outl %[value], %[port]" : : [port]"Nd"(p), [value]"a"(v));
  }


  [[gnu::always_inline]]
  inline uint8_t inb(const port_t p) {
    uint8_t r = 0;
    asm volatile("inb %[port], %[res]" : [res]"=a"(r) : [port]"Nd"(p));
    return r;
  }

  [[gnu::always_inline]]
  inline uint16_t inw(const port_t p) {
    uint16_t r = 0;
    asm volatile("inw %[port], %[res]" : [res]"=a"(r) : [port]"Nd"(p));
    return r;
  }

  [[gnu::always_inline]]
  inline uint32_t ind(const port_t p) {
    uint32_t r = 0;
    asm volatile("inl %[port], %[res]" : [res]"=a"(r) : [port]"Nd"(p));
    return r;
  }
}
