// Copyright (c) 2016 Kulshan Concepts. All rights reserved.
#pragma once

#include <stddef.h>
#include <stdint.h>

size_t strlen(const char* str);
void itoa(int i, char* buffer);
void itoa_hex(uint32_t i, char* buffer);
extern "C" void* memcpy(void* destination, const void* source, size_t count);
