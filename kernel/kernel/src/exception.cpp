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

#include "exception.h"
#include "mmio.h"
#include "thread.h"

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

/** Get the SPSR register
 */
uint32_t getSpsr() {
	uint32_t r;
	asm("mrs %[ps], spsr" : [ps]"=r" (r));
	return r;
}

void enableFIQ() {
	setCpsr(getCpsr() & ~(1 << 6));
}

void enableIRQ() {
	setCpsr(getCpsr() & ~(1 << 7));
}

static bool firstSwitch = true;

void exceptionHandler(uint32_t lr, uint32_t type) {

	if (type == ARM4_XRQ_IRQ) {
		// check that the IRQ is indeed activated
		if (mmio_read(0x14000000 + PIC_IRQ_STATUS * 4) & 0x20) {
			// clear it
			mmio_write(0x13000000 + REG_INTCLR * 4, 1);

			Thread* thread;

			// The very first time we get this interrupt, we need to initialize
			// the state a little bit.
			if (firstSwitch) {
				firstSwitch = false;

				thread = Thread::getCurrentThread();
				thread->pc = ((uint32_t*)KERNEL_EXCEPTION_STACK)[-1];
				thread->r12 = ((uint32_t*)KERNEL_EXCEPTION_STACK)[-2];
				thread->r11 = ((uint32_t*)KERNEL_EXCEPTION_STACK)[-3];
				thread->r10 = ((uint32_t*)KERNEL_EXCEPTION_STACK)[-4];
				thread->r9 = ((uint32_t*)KERNEL_EXCEPTION_STACK)[-5];
				thread->r8 = ((uint32_t*)KERNEL_EXCEPTION_STACK)[-6];
				thread->r7 = ((uint32_t*)KERNEL_EXCEPTION_STACK)[-7];
				thread->r6 = ((uint32_t*)KERNEL_EXCEPTION_STACK)[-8];
				thread->r5 = ((uint32_t*)KERNEL_EXCEPTION_STACK)[-9];
				thread->r4 = ((uint32_t*)KERNEL_EXCEPTION_STACK)[-10];
				thread->r3 = ((uint32_t*)KERNEL_EXCEPTION_STACK)[-11];
				thread->r2 = ((uint32_t*)KERNEL_EXCEPTION_STACK)[-12];
				thread->r1 = ((uint32_t*)KERNEL_EXCEPTION_STACK)[-13];
				thread->r0 = ((uint32_t*)KERNEL_EXCEPTION_STACK)[-14];
				thread->cpsr = ((uint32_t*)KERNEL_EXCEPTION_STACK)[-15];

				// To get the sp and lr we need to go into system mode
				uint32_t savedSP;
				uint32_t savedLR;

				asm("mrs r0, cpsr \n"
					"bic r0, r0, #0x1f \n"
					"orr r0, r0, #0x1f \n"
					"msr cpsr, r0 \n" /* now in system mode */
					"mov %[sp], sp \n"
					"mov %[lr], lr \n"
					"bic r0, r0, #0x1f \n"
					"orr r0, r0, #0x12 \n"
					"msr cpsr, r0 \n" /* and back to exception mode */
					: [sp]"=r" (savedSP), [lr]"=r" (savedLR));

				thread->sp = savedSP;
				thread->lr = savedLR;
			} else {
				thread = Thread::getNextReady();
			}

			// Now reaturing to a thread. We need to restore its registers.
			((uint32_t*)KERNEL_EXCEPTION_STACK)[-1] = thread->pc;
			((uint32_t*)KERNEL_EXCEPTION_STACK)[-2] = thread->r12;
			((uint32_t*)KERNEL_EXCEPTION_STACK)[-3] = thread->r11;
			((uint32_t*)KERNEL_EXCEPTION_STACK)[-4] = thread->r10;
			((uint32_t*)KERNEL_EXCEPTION_STACK)[-5] = thread->r9;
			((uint32_t*)KERNEL_EXCEPTION_STACK)[-6] = thread->r8;
			((uint32_t*)KERNEL_EXCEPTION_STACK)[-7] = thread->r7;
			((uint32_t*)KERNEL_EXCEPTION_STACK)[-8] = thread->r6;
			((uint32_t*)KERNEL_EXCEPTION_STACK)[-9] = thread->r5;
			((uint32_t*)KERNEL_EXCEPTION_STACK)[-10] = thread->r4;
			((uint32_t*)KERNEL_EXCEPTION_STACK)[-11] = thread->r3;
			((uint32_t*)KERNEL_EXCEPTION_STACK)[-12] = thread->r2;
			((uint32_t*)KERNEL_EXCEPTION_STACK)[-13] = thread->r1;
			((uint32_t*)KERNEL_EXCEPTION_STACK)[-14] = thread->r0;
			((uint32_t*)KERNEL_EXCEPTION_STACK)[-15] = thread->cpsr;

			// again, we need system mode to get sp and lr
			asm("mrs r0, cpsr \n"
				"bic r0, r0, #0x1f \n"
				"orr r0, r0, #0x1f \n"
				"msr cpsr, r0 \n" /* now in system mode */
				"mov sp, %[sp] \n"
				"mov lr, %[lr] \n"
				"bic r0, r0, #0x1f \n"
				"orr r0, r0, #0x12 \n"
				"msr cpsr, r0 \n" /* and back to exception mode */
				: [sp]"=r" (thread->sp), [lr]"=r" (thread->lr));
		}
	} else if (type == ARM4_XRQ_SWINT) {
		uint32_t swi = ((uint32_t*)(lr - 4))[0] & 0xffff;
		// TODO: call a handler based on what index was specified (now in "swi")
		(void) swi;
	} else {
		//kprint("Unhandled exception; stop.");
		while (1) { }
	}
}

#define EXCEPTION_TOP_SWI \
	uint32_t lr; \
	asm("mov sp, %[ps]" : : [ps]"i" (KERNEL_EXCEPTION_STACK)); /* set sp(r13) */ \
	asm("push {lr}"); /* save lr(r14) */ \
	asm("push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12}"); /* save others */ \
	asm("mov %[ps], lr" : [ps]"=r" (lr)); /* get new lr */

#define EXCEPTION_BOTTOM_SWI \
	asm("pop {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12}"); /* restore registers */ \
	asm("ldm sp!, {pc}^"); /* special mode to return from exception handlers */

#define EXCEPTION_TOP \
	uint32_t lr; \
	asm("mov sp, %[ps]" : : [ps]"i" (KERNEL_EXCEPTION_STACK)); /* set sp */ \
	asm("sub lr, lr, #4"); \
	asm("push {lr}"); /* save lr */ \
	asm("push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12}"); /* save others */ \
	asm("mrs r0, spsr"); /* get spsr to save it */ \
	asm("push {r0}");  /* save spsr */ \
	asm("mov %[ps], lr" : [ps]"=r" (lr)); /* get the new lr */

#define EXCEPTION_BOTTOM \
	asm("pop {r0}"); /* get old value of spsr */ \
	asm("msr spsr, r0"); /* set spsr back */ \
	asm("pop {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12}"); /* restore others */ \
	asm("ldm sp!, {pc}^"); /* special mode to return from exception handlers */

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

	// enable timer IRQ
	mmio_write(0x14000000 + PIC_IRQ_ENABLESET * 4, (1 << 5) | (1 << 6) | (1 << 7));

	// initialize timer
	mmio_write(0x13000000 + REG_LOAD * 4, 0xFFFFFF);
	mmio_write(0x13000000 + REG_BGLOAD * 4, 0xFFFFFF);
	mmio_write(0x13000000 + REG_CTRL * 4, CTRL_ENABLE | CTRL_MODE_PERIODIC
		| CTRL_SIZE_32 | CTRL_DIV_NONE | CTRL_INT_ENABLE);
	mmio_write(0x13000000 + REG_INTCLR, 0xFFFFFFFF);
}
