// Copyright (C) 2016 Kulshan Concepts. All rights reserved.

#include <stddef.h>
#include <stdint.h>

#include "uart.h"
#include "print.h"
#include "atag.h"
#include "memory.h"
#include "exception.h"
#include "logger.h"
#include "sdp.h"

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

	logger.info(MODULE, "Kernel is starting with parameters %X %X %X", r0, r1, atagsAddress);

	logger.debug(MODULE, "Reading ATAG information from %X...", atagsAddress);

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
				logger.debug(MODULE, "Memory ATAG entry (%d MB @ 0x%X)", atag->mem.size >> 20, atag->mem.startAddress);
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

	logger.debug(MODULE, "The kernel starts at 0x%X and is %d KB.", kernelStart, kernelSize >> 10);

	Exceptions exceptions;

	exceptions.enableExceptions();

	logger.fatal(MODULE, "End of execution");

	// TODO: can we halt or something?
	while (true) {}
}
