// Copyright (C) 2016 Kulshan Concepts. All rights reserved.

.section ".text.boot"

.globl _start_bl

// Kernel entry point
// r15 = should be 0x8000
// r0  = 0x00000000
// r1  = 0x00000C42
// r2  = 0x00000100  (start of ATAGS)
// we must preserve these for the kernel itself
_start_bl:
	// stack setup (grows back from 0x8000)
	mov sp, #0x8000

	// relocate the bootloader
	mov r3, #0x8000
	ldr r4, =__start
	ldr r9, =__data_end
1:
	ldmia r3!, {r5-r8}
	stmia r4!, {r5-r8}

	cmp r4, r9
	blo 1b

	// Code is now relocated

	// Clear out the BSS
	ldr r4, =__bss_start
	ldr r9, =__bss_end
	mov r5, #0
	mov r6, #0
	mov r7, #0
	mov r8, #0
	b 2f

1:
	// store four zeroes starting at bss_start
	stmia r4!, {r5-r8}
	
2:
	// ... until we're at the end of the bss
	cmp r4, r9
	blo 1b

	// get address of kernel_main
	ldr r3, =kernel_main // this will actually jump us to our relocated location
	blx r3

halt:
	wfe
	b halt

