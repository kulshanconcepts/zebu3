// Copyright (c) 2016 Kulshan Concepts. All rights reserved.

#include "mmio.h"

#define RPI2_BASE 0x3F000000

void mmio_write(uint32_t reg, uint32_t data) {
	*(volatile uint32_t*)(reg + RPI2_BASE) = data;
}

uint32_t mmio_read(uint32_t reg) {
	return *(volatile uint32_t*)(reg + RPI2_BASE);
}
