#pragma once

#include "stdint.hpp"

int strcmp(const char* l, const char* r) asm("strcmp");
int strncmp(const char* l, const char* r, size_t c) asm("strncmp");

void* memcpy(void* dist, const void* src, size_t len) asm("memcpy");
void* memset(void* dist, int ch, size_t len) asm("memset");
