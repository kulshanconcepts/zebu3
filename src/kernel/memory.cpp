// Copyright (c) 2016 Kulshan Concepts. All rights reserved.

#include "memory.h"

void memset(void* address, uint32_t value, size_t count) {
	uint8_t* addr = (uint8_t*)address;

	while (count >= 4) {
		*((uint32_t*)addr) = value;
		addr += 4;
		count -= 4;
	}

	while (count > 0) {
		*addr = (uint8_t)value;
		addr++;
		count--;
	}
}

#define UNINITIALIZED_BLOCK_START 0xFFFFFFFF

#define ALLOC_SET(index) (allocations[index >> 5] |= (1 << (index & 0x1F))); freeCount--
#define ALLOC_UNSET(index) (allocations[index >> 5] &= ~(1 << (index & 0x1F))); freeCount++
#define IS_ALLOC(index) (allocations[index >> 5] & (1 << (index & 0x1F)))

PhysicalMemory::PhysicalMemory() : allocations(NULL), pageSize(0), freeCount(0), totalCount(0) {
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

void PhysicalMemory::initialize(uint32_t kernelStart, uint32_t kernelSize, uint32_t pageSize) {
	this->pageSize = pageSize;

	uint32_t kernelPages = kernelSize / pageSize;
	if (kernelSize % pageSize != 0) {
		kernelPages++;
	}

	uint32_t totalPages = 0;

	for (size_t i = 0; i < MAX_PHYSICAL_BLOCKS; i++) {
		if (blocks[i].start != UNINITIALIZED_BLOCK_START) {
			uint32_t end = blocks[i].start + blocks[i].length;
			if ((end / pageSize) > totalPages) {
				totalPages = end / pageSize;
			}
		}
	}

	uint32_t bytesOfAllocation = (totalPages % 8 == 0) ? totalPages / 8 : totalPages / 8 + 1;
	uint32_t pagesOfAllocation = (bytesOfAllocation % pageSize == 0) ? bytesOfAllocation / pageSize : bytesOfAllocation / pageSize + 1;

	uint32_t kernelFirstPage = kernelStart / pageSize;

	// adjust the Kernel, the stack is just before it
	kernelFirstPage--;
	kernelPages++;

	// we need pagesOfAllocation consecutive pages to store our allocation bits
	// if this can fit before the kernel, we'll put it there, but leave a page at the beginning reserved
	// otherwise, it goes after the kernel
	uint32_t allocFirstPage = kernelFirstPage + kernelPages;

	if (pagesOfAllocation < kernelFirstPage) {
		allocFirstPage = 1; // leave the first page open, cuz it has stuff there
	}

	allocations = (uint32_t*)(allocFirstPage * pageSize);

	// clear it out (everythign is available)
	memset(allocations, bytesOfAllocation, 0);

	// TODO: the follow may not be true if there are holes (more than one block)
	totalCount = totalPages;
	freeCount = totalCount;

	ALLOC_SET(0);

	for (uint32_t idx = allocFirstPage; idx < allocFirstPage + pagesOfAllocation; idx++) {
		ALLOC_SET(idx);
	}

	for (uint32_t idx = kernelFirstPage; idx < kernelFirstPage + kernelPages; idx++) {
		ALLOC_SET(idx);
	}
}

uint32_t PhysicalMemory::allocatePage() {
	// TODO: detect out of memory
	uint32_t idx = 1;
	while (IS_ALLOC(idx)) { idx++; }
	ALLOC_SET(idx);

	return idx * pageSize;
}

void PhysicalMemory::freePage(uint32_t address) {
	ALLOC_UNSET(address / pageSize);
}

uint32_t PhysicalMemory::getTotalMemory() const {
	return totalCount * pageSize;
}

uint32_t PhysicalMemory::getFreeMemory() const {
	return freeCount * pageSize;
}

KernelHeap::KernelHeap(PhysicalMemory* const physicalMemory) : physicalMemory(physicalMemory) {
	freeBlocks = (KernelHeapBlock*)physicalMemory->allocatePage();
	freeBlocks->size = physicalMemory->getPageSize() - sizeof(KernelHeapBlock);
	freeBlocks->next = NULL;
	freeBlocks->prev = NULL;
	usedBlocks = NULL;
	usedBytes = sizeof(KernelHeapBlock);
	freeBytes = freeBlocks->size;
	instance = this;
}

KernelHeap* KernelHeap::instance = NULL;

void* KernelHeap::allocate(size_t length) {
	// find a free block that's either EXACTLY the right size, or big enough for length and a structure
	KernelHeapBlock* block = freeBlocks;
	length += 4 - (length % 4); // keep all memory addresses aligned on 32-bit boundaries

	while (1) {
		if (block->size >= length && block->size < length + sizeof(KernelHeapBlock) + 4) {
			
			// move it to the used list and take it from the free list
			if (block->prev != NULL) {
				block->prev->next = block->next;
			}

			if (block->next != NULL) {
				block->next->prev = block->prev;
			}

			break;
		} else if (block->size >= length + sizeof(KernelHeapBlock) + 4) {
			// it's big enough to split up
			// split it into two parts, one for the used list and one for the free list
			KernelHeapBlock* newFree = (KernelHeapBlock*)((uint32_t)block + length + sizeof(KernelHeapBlock));
			newFree->size = block->size - length - sizeof(KernelHeapBlock);
			newFree->next = block->next;
			newFree->prev = block->prev;

			if (block->prev) {
				block->prev->next = newFree;
			}

			if (block->next) {
				block->next->prev = newFree;
			}

			// special case where the block we're splitting is the first one
			if (block == freeBlocks) {
				freeBlocks = newFree;
			}

			// the new block is now in place in the free list, now we just adjust ours into the used list
			block->size = length;

			freeBytes -= sizeof(KernelHeapBlock); // created a new one
			usedBytes += sizeof(KernelHeapBlock);

			break;
		} else {
			block = block->next;
			if (block == NULL) {
				// out of memory
				return NULL;
			}
		}
	}

	block->next = usedBlocks;
	block->prev = NULL;
	if (usedBlocks != NULL) {
		usedBlocks->prev = block;
	}
	usedBlocks = block;

	freeBytes -= block->size;
	usedBytes += block->size;

	return (void*)((uint32_t)block + sizeof(KernelHeapBlock));
}

void KernelHeap::free(void* ptr) {
	// TODO: at some point we need to combine consecutive free blocks back into bigger free blocks
	// but since the list is out of order, that could be... fun

	KernelHeapBlock* block = (KernelHeapBlock*)((uint32_t)ptr - sizeof(KernelHeapBlock));

	if (block->prev != NULL) {
		block->prev->next = block->next;
	}

	if (block->next != NULL) {
		block->next->prev = block->prev;
	}

	block->next = freeBlocks;
	block->prev = NULL;
	if (freeBlocks != NULL) {
		freeBlocks->prev = block;
	}
	freeBlocks = block;

	freeBytes += block->size + sizeof(KernelHeapBlock);
	usedBytes -= block->size - sizeof(KernelHeapBlock);
}
