#pragma once

#include <stdarg.h>

namespace term {
  static volatile char* const vmem_base = reinterpret_cast<volatile char*>(0xb8000);
  constexpr int width = 80;
  constexpr int height = 25;

  void clear();
  void clearLine();
  void init(); // fixme: if not init will corrupt mem

  int getX();
  int getY();

  void moveAbs(int x, int y);
  void moveRel(int dx, int dy);
  void nl();

  void putch(const char x);
  void puts(const char* str);


  void putnum(signed char x, const signed char base /*= 10*/);
  void putnum(short       x, const short       base /*= 10*/);
  void putnum(int         x, const int         base /*= 10*/);
  void putnum(long        x, const long        base /*= 10*/);
  void putnum(long long   x, const long long   base /*= 10*/);

  void putnum(unsigned char      x, const unsigned char      base /*= 10*/);
  void putnum(unsigned short     x, const unsigned short     base /*= 10*/);
  void putnum(unsigned int       x, const unsigned int       base /*= 10*/);
  void putnum(unsigned long      x, const unsigned long      base /*= 10*/);
  void putnum(unsigned long long x, const unsigned long long base /*= 10*/);


  void putsln(const char* str);
  [[gnu::format(printf, 1, 2)]]
  void printf(const char* fmt...);


  void panicln(const char* str);
}
