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

	while ((atag = atags.getNextTag()) != NULL) {
		switch (atag->tag) {
			case ATAG_CORE:
				if (atag->size == 2) {
					kprintf(" core (empty)");
				} else {
					kprintf(" core (flags %X, page size %d, root device %d)", atag->core.flags, atag->core.pageSize, atag->core.rootDevice);
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
 
	kprint("\r\nEnd of execution\r\n");

	// Just start echoing anything that's typed
	while (true)
		uart.putc(uart.getc());
}

