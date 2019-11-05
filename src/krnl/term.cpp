#include "hpp/term.hpp"

#include <stdarg.h>

#include "hpp/typ.hpp"

namespace term {
  static int posX = 0;
  static int posY = 0;
  static volatile char* vmem_woffset = nullptr;

  void clear() {
    vmem_woffset = vmem_base;
    for (int x = 0; x < width; ++x)
      for (int y = 0; y < height; ++y) {
        *vmem_woffset++ = ' ';
        *vmem_woffset++ = 7;
      }

    posX = 0;
    posY = 0;
    vmem_woffset = vmem_base;
  }
  void clearLine() {
    moveAbs(0, posY);
    for (int x = 0; x < width; ++x) {
      *vmem_woffset++ = ' ';
      *vmem_woffset++ = 7;
    }
    moveAbs(0, posY);
  }
  void init() {
    posX = 0;
    posY = 0;
    vmem_woffset = vmem_base;
  }

  int getX() {
    return posX;
  }
  int getY() {
    return posY;
  }

  void moveAbs(int x, int y) {
    posX = x%width;
    posY = y%height + x/width;

    vmem_woffset = vmem_base + (posY*width + posX)*2; // (char, style)
  }
  void moveRel(int dx, int dy) {
    moveAbs(posX+dx, posY+dy);
  }
  void nl() {
    moveAbs(0, posY+1);
  }


  void putch(const char c) {
    if (c == '\n') {
      nl();
      return;
    }
    else if (c == '\r') {
      moveAbs(0, posY);
      return;
    }

    *vmem_woffset++ = c;
    *vmem_woffset++ = 7;

    moveRel(1, 0);
  }

  void puts(const char* str) {
    while (*str != '\0')
      putch(*str++);
  }


  constexpr char lcase_alphabet[] = "0123456789abcdef";
  constexpr char ucase_alphabet[] = "0123456789ABCDEF";
  template<class T, const char* alphabet = lcase_alphabet>
  typ::enable_if< typ::unsigned_::pred<T> > putnum(T x, const T base = 10) {
    // if (base == 16)
    //   puts("0x");
    // else if (base == 2)
    //   puts("0b");

    if (x == 0)
      putch('0');
    else {
      // if (base == 8)
      //   putch('0');

      volatile char* start = vmem_woffset;
      for (;x > 0; x /= base)
        putch(alphabet[x%base]);

      // reverse the number
      volatile char* cur = vmem_woffset-2; /*take last char*/
      while (cur > start) {
        char tmp = *cur;
        *cur = *start;
        *start = tmp;

        cur -= 2; /*(char, style)*/
        start += 2;
      }
    }
  }
  template<class T, const char* alphabet = lcase_alphabet>
  typ::enable_if< typ::signed_::pred<T> > putnum(const T x, const T base = 10) {
    using unsignedT = typ::unsigned_::make<T>;

    if (x >= 0) {
      putnum<unsignedT, alphabet>(static_cast<unsignedT>(x), static_cast<unsignedT>(base));
      return;
    }

    putch('-');
    putnum<unsignedT, alphabet>(static_cast<unsignedT>(-x), static_cast<unsignedT>(base));
  }
  template void putnum<signed char>(signed char x, const signed char base);
  template void putnum<short>(short x, const short base);
  template void putnum<int>(int x, const int base);
  template void putnum<long>(long x, const long base);
  template void putnum<long long>(long long x, const long long base);

  template void putnum<unsigned char>(unsigned char x, const unsigned char base);
  template void putnum<unsigned short>(unsigned short x, const unsigned short base);
  template void putnum<unsigned int>(unsigned int x, const unsigned int base);
  template void putnum<unsigned long>(unsigned long x, const unsigned long base);
  template void putnum<unsigned long long>(unsigned long long x, const unsigned long long base);


  void putsln(const char* str) {
    puts(str);
    putch('\n');
  }
  void printf(const char* fmt...) {
    va_list args;
    va_start(args, fmt);

    for (;*fmt != '\0'; ++fmt) {
      if (*fmt == '%') {
        ++fmt;

        bool ljustify = false;
        bool force_sign = false;
        bool alt = false;
        bool padw0 = false;
        while (true) {
          if (*fmt == '-')
            ljustify = true;
          else if (*fmt == '+')
            force_sign = true;
          else if (*fmt == '#')
            alt = true;
          else if (*fmt == '0')
            padw0 = true;
          else break;

          ++fmt;
        }

        int minw = -1;
        if (*fmt == '*') {
          minw = va_arg(args, int);
          ++fmt;
        }
        else if ('0' <= *fmt && *fmt <= '9') {
          minw = 0;
          while ('0' <= *fmt && *fmt <= '9') {
            minw *= 10;
            minw += *fmt - '0';
            ++fmt;
          }
        }

        int prec = -1;
        if (*fmt == '.') {
          prec = 0;
          ++fmt;

          if (*fmt == '*') {
            prec = va_arg(args, int);
            ++fmt;
          }
          else
            while ('0' <= *fmt && *fmt <= '9') {
              prec *= 10;
              prec += *fmt - '0';
              ++fmt;
            }
        }

        bool hh = false;
        bool h  = false;
        bool l  = false;
        bool ll = false;
        if (*fmt == 'h') {
          h = true;
          ++fmt;

          if (*fmt == 'h') {
            hh = true;
            ++fmt;
          }
        }
        else if (*fmt == 'l') {
          l = true;
          ++fmt;

          if (*fmt == 'l') {
            ll = true;
            ++fmt;
          }
        }

        //
        // note: va_args: types < int as targets are undefbeh
        //

        if (*fmt == '%')
          putch('%');
        else if (*fmt == 'c')
          putch( static_cast<const char>(va_arg(args, int)) );
        else if (*fmt == 's')
          puts(va_arg(args, const char*));
        else if (*fmt == 'd' || *fmt == 'i') {
          if (hh)
            putnum<signed char>( static_cast<signed char>(va_arg(args, int)) );
          else if (h)
            putnum<short>( static_cast<short>(va_arg(args, int)) );
          else if (ll)
            putnum<long long>(va_arg(args, long long));
          else if (l)
            putnum<long>(va_arg(args, long));
          else
            putnum<int>(va_arg(args, int));
        }
        else if (*fmt == 'o') {
          if (hh)
            putnum<unsigned char>( static_cast<unsigned char>(va_arg(args, unsigned int)), 8);
          else if (h)
            putnum<unsigned short>( static_cast<unsigned short>(va_arg(args, unsigned int)), 8);
          else if (ll)
            putnum<unsigned long long>(va_arg(args, unsigned long long), 8);
          else if (l)
            putnum<unsigned long>(va_arg(args, unsigned long), 8);
          else
            putnum<unsigned int>(va_arg(args, unsigned int), 8);
        }
        else if (*fmt == 'x' || *fmt == 'p') {
          if (hh)
            putnum<unsigned char>( static_cast<unsigned char>(va_arg(args, unsigned int)), 16);
          else if (h)
            putnum<unsigned short>( static_cast<unsigned short>(va_arg(args, unsigned int)), 16);
          else if (ll)
            putnum<unsigned long long>(va_arg(args, unsigned long long), 16);
          else if (l)
            putnum<unsigned long>(va_arg(args, unsigned long), 16);
          else
            putnum<unsigned int>(va_arg(args, unsigned int), 16);
        }
        else if (*fmt == 'X') {
          if (hh)
            putnum<unsigned char, ucase_alphabet>( static_cast<unsigned char>(va_arg(args, unsigned int)), 16);
          else if (h)
            putnum<unsigned short, ucase_alphabet>( static_cast<unsigned short>(va_arg(args, unsigned int)), 16);
          else if (ll)
            putnum<unsigned long long, ucase_alphabet>(va_arg(args, unsigned long long), 16);
          else if (l)
            putnum<unsigned long, ucase_alphabet>(va_arg(args, unsigned long), 16);
          else
            putnum<unsigned int, ucase_alphabet>(va_arg(args, unsigned int), 16);
        }
        else if (*fmt == 'u') {
          if (hh)
            putnum<unsigned char>( static_cast<unsigned char>(va_arg(args, unsigned int)) );
          else if (h)
            putnum<unsigned short>( static_cast<unsigned short>(va_arg(args, unsigned int)) );
          else if (ll)
            putnum<unsigned long long>(va_arg(args, unsigned long long));
          else if (l)
            putnum<unsigned long>(va_arg(args, unsigned long));
          else
            putnum<unsigned int>(va_arg(args, unsigned int));
        }
        // else // invalid format character
      }
      else
        putch(*fmt);
    }

    va_end(args);
  }
}
