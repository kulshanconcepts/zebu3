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
#include "print.h"
#include "atag.h"
#include "memory.h"
#include "exception.h"
#include "logger.h"
#include "sdp.h"
#include "version.h"
#include "thread.h"
#include "led.h"

extern "C" uint32_t __start;
extern "C" uint32_t __end;

#define MODULE "KernelMain"

extern "C"
void kernel_main(uint32_t r0, uint32_t r1, uint32_t atagsAddress) {
	(void) r0;
	(void) r1;

	uint32_t kernelStart = (uint32_t)&__start;
	uint32_t kernelSize = (uint32_t)&__end - kernelStart;

 	Uart uart;

	SdpServer sdpServer(uart);

	sdpServer.connect();

	Logger logger(sdpServer);

	logger.info(MODULE, PRODUCT_NAME " " PRODUCT_VERSION_STR
			": kernel is starting with parameters %X %X %X", r0, r1, atagsAddress);

	logger.debug(MODULE, "Reading ATAG information from %p...", atagsAddress);

	PhysicalMemory physicalMemory;

	ATags atags(atagsAddress);
	ATagDescriptor* atag;

	uint32_t coreFlags = 0;
	uint32_t pageSize = 4096;
	uint32_t rootDevice = 0;

	while ((atag = atags.getNextTag()) != NULL) {
		switch (atag->tag) {
			case ATAG_CORE:
				if (atag->size == 2) {
					logger.debug(MODULE, "Empty Core ATAG entry");
				} else {
					coreFlags = atag->core.flags;
					pageSize = atag->core.pageSize == 0 ? pageSize : atag->core.pageSize;
					rootDevice = atag->core.rootDevice;
					logger.debug(MODULE, "Core ATAG entry (flags %X, page size %d, root device %d)", coreFlags, pageSize, rootDevice);
				}
				break;
			case ATAG_MEM:
				logger.debug(MODULE, "Memory ATAG entry (%d MB @ %p)", atag->mem.size >> 20, atag->mem.startAddress);
				if (!physicalMemory.addBlock(atag->mem.startAddress, atag->mem.size)) {
					logger.error(MODULE, "Could not add memory block to list (too many?)");
				}
				break;
			default:
				logger.debug(MODULE, "Unknown ATAG type %X", atag->tag);
		}
	}

	logger.debug(MODULE, "Initializing memory map");

	physicalMemory.initialize(kernelStart, kernelSize, pageSize);

	logger.info(MODULE, "Starting with %d KB free of %d KB total memory.", physicalMemory.getFreeMemory() >> 10, physicalMemory.getTotalMemory() >> 10);

	KernelHeap heap(&physicalMemory);

	logger.info(MODULE, "Kernel heap has %d bytes free and %d bytes used.", heap.getFreeBytes(), heap.getUsedBytes());

	logger.debug(MODULE, "The kernel starts at %p and is %d KB.", kernelStart, kernelSize >> 10);

	RaspiLed led;
	led.turnOff();

	Exceptions exceptions;

	Thread::initialize(); // creates the idle thread

	exceptions.enableExceptions();

	while (true) {
		logger.info(MODULE, "Sitting like a duck");
		//asm("wfi");
	} // idle forever
}
