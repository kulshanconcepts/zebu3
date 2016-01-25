// Copyright (c) 2016 Kulshan Concepts. All rights reserved.

#include "exception.h"
#include "mmio.h"

/** Get the CPSR register
 */
uint32_t getCpsr() {
	uint32_t r;
	asm("mrs %[ps], cpsr" : [ps]"=r" (r));
	return r;
}

/** Set the CPSR register
 */
 void setCpsr(uint32_t r) {
 	asm("msr cpsr, %[ps]" : : [ps]"r" (r));
 }

void enableFIQ() {
	setCpsr(getCpsr() & ~(1 << 6));
}

void enableIRQ() {
	setCpsr(getCpsr() & ~(1 << 7));
}

void exceptionHandler(uint32_t lr, uint32_t type) {
	// clear the interrupt
	mmio_write(0x13000000 + REG_INTCLR * 4, 1);

	if (type == ARM4_XRQ_SWINT) { // software interrupt
		uint32_t swi = ((uint32_t*)((uint32_t*)lr - 4))[0] & 0xffff;

		// TODO: call a handler based on what index was specified (now in "swi")
		(void) swi;
	} else if (type != ARM4_XRQ_IRQ && type != ARM4_XRQ_FIQ) {
		//kprint("Unhandled exception; stop.");
		while (1) { }
	}
}

#define EXCEPTION_TOP_SWI \
	uint32_t lr; \
	asm("mov sp, %[ps]" : : [ps]"i" (KERNEL_EXCEPTION_STACK)); \
	asm("push {lr}"); \
	asm("push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12}"); \
	asm("mov %[ps], lr" : [ps]"=r" (lr));

#define EXCEPTION_TOP \
	uint32_t lr; \
	asm("mov sp, %[ps]" : : [ps]"i" (KERNEL_EXCEPTION_STACK)); \
	asm("sub lr, lr, #4"); \
	asm("push {lr}"); \
	asm("push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12}"); \
	asm("mov %[ps], lr" : [ps]"=r" (lr));

#define EXCEPTION_BOTTOM \
	asm("pop {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12}"); \
	asm("ldm sp!, {pc}^");

void __attribute__((naked)) exceptionIRQEntry() { EXCEPTION_TOP exceptionHandler(lr, ARM4_XRQ_IRQ); EXCEPTION_BOTTOM }
void __attribute__((naked)) exceptionFIQEntry() { EXCEPTION_TOP exceptionHandler(lr, ARM4_XRQ_FIQ); EXCEPTION_BOTTOM }
void __attribute__((naked)) exceptionRestetEntry() { EXCEPTION_TOP exceptionHandler(lr, ARM4_XRQ_RESET); EXCEPTION_BOTTOM }
void __attribute__((naked)) exceptionUndefinedEntry() { EXCEPTION_TOP exceptionHandler(lr, ARM4_XRQ_UNDEF); EXCEPTION_BOTTOM }
void __attribute__((naked)) exceptionAbortPEntry() { EXCEPTION_TOP exceptionHandler(lr, ARM4_XRQ_ABRTP); EXCEPTION_BOTTOM }
void __attribute__((naked)) exceptionAbortDEntry() { EXCEPTION_TOP exceptionHandler(lr, ARM4_XRQ_ABRTD); EXCEPTION_BOTTOM }
void __attribute__((naked)) exceptionSWIEntry() { EXCEPTION_TOP_SWI exceptionHandler(lr, ARM4_XRQ_SWINT); EXCEPTION_BOTTOM }

void installExceptionHandler(uint32_t index, void(*address)()) {
	uint32_t* vectorTable = (uint32_t*)0;
	vectorTable[index] = 0xEA000000 | (((uint32_t)address - (8 + 4 * index)) >> 2);
}

Exceptions::Exceptions() {
	installExceptionHandler(ARM4_XRQ_IRQ, &exceptionIRQEntry);
	installExceptionHandler(ARM4_XRQ_FIQ, &exceptionIRQEntry);
	installExceptionHandler(ARM4_XRQ_RESET, &exceptionIRQEntry);
	installExceptionHandler(ARM4_XRQ_UNDEF, &exceptionIRQEntry);
	installExceptionHandler(ARM4_XRQ_ABRTP, &exceptionIRQEntry);
	installExceptionHandler(ARM4_XRQ_ABRTD, &exceptionIRQEntry);
	installExceptionHandler(ARM4_XRQ_SWINT, &exceptionIRQEntry);
}

void Exceptions::enableExceptions() {
	enableIRQ();
}
