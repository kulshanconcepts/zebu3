// Copyright (c) 2016 Kulshan Concepts. All rights reserved.

#include <stddef.h>
#include <stdint.h>

#define MAX_PHYSICAL_BLOCKS 10

void memset(void* address, uint32_t value, size_t count);

struct physical_block {
	uint32_t start;
	uint32_t length;
};

class PhysicalMemory {
private:
	physical_block blocks[MAX_PHYSICAL_BLOCKS];
	uint32_t* allocations;
	uint32_t pageSize;
	uint32_t freeCount;
	uint32_t totalCount;
public:
	PhysicalMemory();
	bool addBlock(uint32_t start, uint32_t length);
	void initialize(uint32_t kernelStart, uint32_t kernelSize, uint32_t pageSize);
	uint32_t allocatePage();
	void freePage(uint32_t address);
	uint32_t getFreeMemory() const;
	uint32_t getTotalMemory() const;
};