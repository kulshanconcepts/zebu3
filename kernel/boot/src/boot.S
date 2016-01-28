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
