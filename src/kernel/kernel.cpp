// Copyright (C) 2016 Kulshan Concepts. All rights reserved.
// This is just something to play with. It should all be replaced with real stuff!

#include <stddef.h>
#include <stdint.h>

#include "uart.h"
#include "print.h"
#include "atag.h"
#include "memory.h"

extern "C"
void kernel_main(uint32_t r0, uint32_t r1, uint32_t atagsAddress) {
	(void) r0;
	(void) r1;
 
 	Uart uart;
	kprint("Zebu\r\n");
	kprintf("Kernel is starting with %X %X %X\r\n", r0, r1, atagsAddress);

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
					pageSize = atag->core.pageSize;
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

	kprint("... done!\r\n");

	// TODO: can we get the kernel start address and size rather than hard-coding?
	physicalMemory.initialize(0x8000, 32 << 10, pageSize);

	kprintf("Starting with %d KB free of %d KB total memory.\r\n", physicalMemory.getFreeMemory() >> 10, physicalMemory.getTotalMemory() >> 10);

	KernelHeap heap(&physicalMemory);

	kprintf("Kernel heap has %d bytes free and %d bytes used.\r\n", heap.getFreeBytes(), heap.getUsedBytes());

	kprint("\r\nEnd of execution\r\n");

	// Just start echoing anything that's typed
	while (true)
		uart.putc(uart.getc());
}

