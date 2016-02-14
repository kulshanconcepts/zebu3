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

#include "thread.h"
#include "memory.h"
#include "gpio.h"
#include "mmio.h"
#include "led.h"

#define ARM4_MODE_USER   0x10
#define ARM4_MODE_FIQ	 0x11
#define ARM4_MODE_IRQ	 0x12
#define ARM4_MODE_SUPER  0x13
#define ARM4_MODE_ABORT	 0x17
#define ARM4_MODE_UNDEF  0x1b
#define ARM4_MODE_SYS    0x1f
#define ARM4_MODE_MON    0x16

Thread* Thread::currentThread = nullptr;

Thread* Thread::runnable = nullptr;
Thread* Thread::blocked = nullptr;

static void idleThread() {
    int counter = 0;

    while (1) {
        counter++;
        if (counter >= 2000000) {
            RaspiLed::getInstance()->toggle();
            counter = 0;
        }
        //asm("wfi"); // sleep until interrupt
    }
}

void Thread::initialize() {
    create((void*)idleThread);
}

Thread::Thread(void* pc) : prev(nullptr), next(nullptr) {
    PhysicalMemory* memory = PhysicalMemory::getInstance();
    this->stackPage = memory->allocatePage();

    this->pc = (uint32_t)pc;
    this->sp = stackPage + memory->getPageSize() - 4;
    this->cpsr = 0x60000000 | ARM4_MODE_SUPER;
    this->next = nullptr;
    this->prev = nullptr;
}

Thread* Thread::create(void* pc) {
    Thread* thread = new Thread(pc);

    run(thread);

    return thread;
}

void Thread::run(Thread* thread) {
    // remove it from whatever list it's in now, if any
    if (thread->next != nullptr) {
        thread->next->prev = thread->prev;
    }
    if (thread->prev != nullptr) {
        thread->prev->next = thread->next;
    }
    // TODO: check any other lists we make
    if (blocked == thread) {
        blocked = thread->next;
    }

    thread->next = runnable;
    thread->prev = nullptr;

    if (runnable != nullptr) {
        runnable->prev = thread;
    }

    runnable = thread;
}

void Thread::block(Thread* thread) {
    // remove it from whatever list it's in now, if any
    if (thread->next != nullptr) {
        thread->next->prev = thread->prev;
    }
    if (thread->prev != nullptr) {
        thread->prev->next = thread->next;
    }
    // TODO: check any other lists we make
    if (runnable == thread) {
        runnable = thread->next;
    }

    thread->next = blocked;
    thread->prev = nullptr;

    if (blocked != nullptr) {
        blocked->prev = thread;
    }

    blocked = thread;
}

Thread* Thread::getNextReady() {
    if (currentThread == runnable) {
        // current thread is at the head of the list, means it's already running
        // we'll rotate the list and get the next one
        Thread* end = runnable;
        while (end->next != nullptr) {
            end = end->next;
        }

        if (end != runnable) {
            end->next = runnable;
            runnable->prev = end;
            Thread* newGuy = runnable->next;
            runnable->next = nullptr;
            runnable = newGuy;
            newGuy->prev = nullptr;
        }
    }

    currentThread = runnable;
    return currentThread;
}
