// Copyright (C) 2016 Kulshan Concepts. All rights reserved.

#include <stddef.h>
#include <stdint.h>

#include "uart.h"
#include "print.h"
#include "atag.h"
#include "memory.h"
#include "exception.h"

extern "C" uint32_t __start;
extern "C" uint32_t __end;

extern "C"
void kernel_main(uint32_t r0, uint32_t r1, uint32_t atagsAddress) {
	(void) r0;
	(void) r1;

	uint32_t kernelStart = (uint32_t)&__start;
	uint32_t kernelSize = (uint32_t)&__end - kernelStart;

 	Uart uart;
 	uart.putc('Z');
	kprint("ebu\n");
	kprintf("Kernel is starting with %X %X %X\n", r0, r1, atagsAddress);

	kprintf("Reading ATAG information from %X...", atagsAddress);

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
					kprintf(" core (empty)");
				} else {
					coreFlags = atag->core.flags;
					pageSize = atag->core.pageSize == 0 ? pageSize : atag->core.pageSize;
					rootDevice = atag->core.rootDevice;
					kprintf(" core (flags %X, page size %d, root device %d)", coreFlags, pageSize, rootDevice);
				}
				break;
			case ATAG_MEM:
				kprintf(" memory (%d MB @ 0x%X)", atag->mem.size >> 20, atag->mem.startAddress);
				if (!physicalMemory.addBlock(atag->mem.startAddress, atag->mem.size)) {
					kprintf(" FAILED");
				}
				break;
			default:
				kprintf(" type %X", atag->tag);
		}
	}

	kprint("... done!\n");

	physicalMemory.initialize(kernelStart, kernelSize, pageSize);

	kprintf("Starting with %d KB free of %d KB total memory.\n", physicalMemory.getFreeMemory() >> 10, physicalMemory.getTotalMemory() >> 10);

	KernelHeap heap(&physicalMemory);

	kprintf("Kernel heap has %d bytes free and %d bytes used.\n", heap.getFreeBytes(), heap.getUsedBytes());

	kprintf("The kernel starts at 0x%X and is %d KB.\n", kernelStart, kernelSize >> 10);

	Exceptions exceptions;

	exceptions.enableExceptions();

	kprint("\nEnd of execution\n");

	// Just start echoing anything that's typed
	while (true)
		uart.putc(uart.getc());
}
