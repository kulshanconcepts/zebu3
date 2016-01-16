// Copyright (C) 2016 Kulshan Concepts. All rights reserved.
// This is just something to play with. It should all be replaced with real stuff!

#if !defined(__cplusplus)
#include <stdbool.h>
#endif
#include <stddef.h>
#include <stdint.h>

#include "uart.h"
#include "print.h"
  
#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main */
#endif
void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags) {
	(void) r0;
	(void) r1;
	(void) atags;
 
 	Uart uart;
	kprintf("Hello, %s World! %d is a nice number; it's %X in hex.\r\n", "Zebu", 42, 42);
 
	// Just start echoing anything that's typed
	while (true)
		uart.putc(uart.getc());
}

