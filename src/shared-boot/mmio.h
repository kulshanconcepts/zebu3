// Copyright (c) 2016 Kulshan Concepts. All rights reserved.
#pragma once

#include <stddef.h>
#include <stdint.h>

void mmio_write(uint32_t reg, uint32_t data);
uint32_t mmio_read(uint32_t reg);
