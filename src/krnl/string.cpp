#include "hpp/string.hpp"

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
