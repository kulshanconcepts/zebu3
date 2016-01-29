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
#pragma once

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
	static PhysicalMemory* instance;
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
	inline uint32_t getPageSize() const { return pageSize; }
	static inline PhysicalMemory* getInstance() { return instance; }
};

struct KernelHeapBlock {
	uint32_t size;
	KernelHeapBlock* prev;
	KernelHeapBlock* next;
};

class KernelHeap {
private:
	static KernelHeap* instance;
	KernelHeapBlock* freeBlocks;
	uint32_t freeBytes;
	KernelHeapBlock* usedBlocks;
	uint32_t usedBytes;
	PhysicalMemory* const physicalMemory;

public:
	KernelHeap(PhysicalMemory* const physicalMemory);
	void* allocate(size_t length);
	void free(void* ptr);

	inline uint32_t getFreeBytes() const { return freeBytes; }
	inline uint32_t getUsedBytes() const { return usedBytes; }

	static inline KernelHeap* getInstance() { return instance; }
};
