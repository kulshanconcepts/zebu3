// Copyright (c) 2016 Kulshan Concepts. All rights reserved.

#include "memory.h"

#define UNINITIALIZED_BLOCK_START 0xFFFFFFFF

PhysicalMemory::PhysicalMemory() {
	for (size_t i = 0; i < MAX_PHYSICAL_BLOCKS; i++) {
		blocks[i].start = UNINITIALIZED_BLOCK_START;
		blocks[i].length = 0x00000000;
	}
}

bool PhysicalMemory::addBlock(uint32_t start, uint32_t length) {
	for (size_t i = 0; i < MAX_PHYSICAL_BLOCKS; i++) {
		if (blocks[i].start == UNINITIALIZED_BLOCK_START) {
			blocks[i].start = start;
			blocks[i].length = length;

			return true;
		}
	}
	return false;
}
