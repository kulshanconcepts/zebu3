/* Copyright (c) 2016, Kulshan Concepts
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. Neither the name of the copyright holder nor the names of its
 *      contributors may be used to endorse or promote products derived from
 *      this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "memory.h"
#include "logger.h"

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

PhysicalMemory* PhysicalMemory::instance = nullptr;

PhysicalMemory::PhysicalMemory() : allocations(NULL), pageSize(0), freeCount(0), totalCount(0) {
	for (size_t i = 0; i < MAX_PHYSICAL_BLOCKS; i++) {
		blocks[i].start = UNINITIALIZED_BLOCK_START;
		blocks[i].length = 0x00000000;
	}

	instance = this;
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

	// adjust the Kernel, the stack is just before it and another stack too!
	kernelFirstPage -= 2;
	kernelPages += 2;

	// we need pagesOfAllocation consecutive pages to store our allocation bits
	// if this can fit before the kernel, we'll put it there, but leave a page at the beginning reserved
	// otherwise, it goes after the kernel
	uint32_t allocFirstPage = kernelFirstPage + kernelPages;

	if (pagesOfAllocation < kernelFirstPage) {
		Logger::getInstance()->debug("PhysicalMemory", "Memory map can fit before the kernel");
		allocFirstPage = 1; // leave the first page open, cuz it has stuff there
	} else {
		Logger::getInstance()->debug("PhysicalMemory", "Memory map can't fit before the kernel, going to page %d", allocFirstPage);
	}

	allocations = (uint32_t*)(allocFirstPage * pageSize);

	// clear it out (everything is available)
	memset(allocations, 0, bytesOfAllocation);

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
	freeBlocks = reinterpret_cast<KernelHeapBlock*>(physicalMemory->allocatePage());
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

void* operator new(size_t size) {
	return KernelHeap::getInstance()->allocate(size);
}

void* operator new[](size_t size) {
	return KernelHeap::getInstance()->allocate(size);
}

void operator delete(void* datum) {
	KernelHeap::getInstance()->free(datum);
}

void operator delete[](void* datum) {
	KernelHeap::getInstance()->free(datum);
}
