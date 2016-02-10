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

#include "mailbox.h"
#include "mmio.h"

#define MAILBOX_BASE 0xB880

enum MailboxStatusBits {
    ARM_MS_FULL = 0x80000000,
    ARM_MS_EMPTY = 0x40000000,
    ARM_MS_LEVEL = 0x400000FF
};

enum MailboxRegisters {
    MB_REG_READ = 0x00,
    MB_REG_PEEK = 0x10,
    MB_REG_SENDER = 0x14,
    MB_REG_STATUS = 0x18,
    MB_REG_CONFIG = 0x1C
};

Mailbox::Mailbox() {

}

void Mailbox::write(MailboxChannels channel, uint32_t value) const {
    value <<= 4; // shift values up by 4 bits to make room for:
    value |= channel;

    while (mmio_read(MAILBOX_BASE + MB_REG_STATUS) & ARM_MS_FULL); // blocked

    mmio_write(MAILBOX_BASE + MB_REG_READ, value);
}

uint32_t Mailbox::read(MailboxChannels channel) const {
    while (true) {
        while (mmio_read(MAILBOX_BASE + MB_REG_STATUS) & ARM_MS_EMPTY); // blocked

        uint32_t value = mmio_read(MAILBOX_BASE + MB_REG_READ);
        if ((value & 0xF) != channel) {
            continue;
        }

        return value >> 4;
    }
}
