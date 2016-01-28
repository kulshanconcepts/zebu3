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

#include <stddef.h>
#include <stdint.h>

#include "uart.h"
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
