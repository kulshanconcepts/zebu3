// Copyright (c) 2016 Kulshan Concepts. All rights reserved.
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

void vsnprintf(char* buffer, size_t bufferSize, const char* format, va_list args);
