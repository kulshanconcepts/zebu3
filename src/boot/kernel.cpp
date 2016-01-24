// Copyright (C) 2016 Kulshan Concepts. All rights reserved.

#include <stddef.h>
#include <stdint.h>

#include "uart.h"
#include "print.h"
#include "atag.h"
#include "sdp.h"

typedef void (*kernelPtr)(uint32_t, uint32_t, uint32_t);

extern "C"
void kernel_main(uint32_t r0, uint32_t r1, uint32_t atagsAddress) {
	(void) r0;
	(void) r1;

    Uart uart;

    ATags atags(atagsAddress);
    ATagDescriptor* atag;

    uint32_t coreFlags = 0;
    uint32_t pageSize = 4096;
    uint32_t rootDevice = 0;

    const size_t MAX_MEMORY_BLOCKS = 10;

    struct memoryBlock {
        uint32_t start;
        uint32_t length;
    } memoryBlocks[MAX_MEMORY_BLOCKS];

    size_t memoryBlockCount = 0;

    while ((atag = atags.getNextTag()) != NULL) {
        switch (atag->tag) {
            case ATAG_CORE:
                if (atag->size != 2) {
                    coreFlags = atag->core.flags;
                    pageSize = atag->core.pageSize == 0 ? pageSize : atag->core.pageSize;
                    rootDevice = atag->core.rootDevice;
                }
                break;
            case ATAG_MEM:
                if (memoryBlockCount < MAX_MEMORY_BLOCKS) {
                    memoryBlocks[memoryBlockCount].start = atag->mem.startAddress;
                    memoryBlocks[memoryBlockCount].length = atag->mem.size;
                    memoryBlockCount++;
                }
                break;
            default:
                break;
        }
    }

    atags.reset();

    BootloaderSdp sdp(uart, atags);

    sdp.run();

    // kernel is now loaded at 0x8000
	kernelPtr kernel = (kernelPtr)0x8000;

	(*kernel)(r0, r1, atagsAddress);

	while (1) {}
}
