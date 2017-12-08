#pragma once

#include "util/gdt.hpp"

namespace gdt {
  static constexpr uint64_t gdt[] = {
    0, // null-selector is invalid, no matter what the entry is, so set to an invalid value
    util::gdt::Entry(
      /*.base = */0,
      /*.limit = */0xffff'ffff,
      /*.ring = */0,
      /*.rw = */true,
      /*.size = */util::gdt::entry::Size::bit32,
      /*.type = */util::gdt::entry::Type::code,
      /*.code_flags = */util::gdt::entry::CodeFlags {
        /*.conforming = */false
      }
    ).bin(),
    util::gdt::Entry(
      /*.base = */0,
      /*.limit = */0xffff'ffff,
      /*.ring = */0,
      /*.rw = */true,
      /*.size = */util::gdt::entry::Size::bit32,
      /*.type = */util::gdt::entry::Type::data,
      /*.data_flags = */util::gdt::entry::DataFlags {
        /*.dir = */util::gdt::entry::Direction::up
      }
    ).bin()
  };
  static constexpr util::gdt::GDTR gdtr = {
    /*.gdt_size = */sizeof(gdt)-1,
    /*.gdt = */gdt
  };

  constexpr unsigned int getSelector(const unsigned int n) {
    return sizeof(gdt[0])*n;
  }

  namespace selector {
    constexpr unsigned int code = getSelector(1);
    constexpr unsigned int data = getSelector(2);
  }
}
