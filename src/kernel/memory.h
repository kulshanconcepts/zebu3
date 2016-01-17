// Copyright (c) 2016 Kulshan Concepts. All rights reserved.

#include <stddef.h>
#include <stdint.h>

#define MAX_PHYSICAL_BLOCKS 10

struct physical_block {
	uint32_t start;
	uint32_t length;
};

class PhysicalMemory {
private:
	physical_block blocks[MAX_PHYSICAL_BLOCKS];
public:
	PhysicalMemory();
	bool addBlock(uint32_t start, uint32_t length);
};
