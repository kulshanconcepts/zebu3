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

#include <stdarg.h>

#include "mailboxproperty.h"
#include "string.h"

static uint32_t propertyTagBuffer[8192] __attribute__((aligned(16)));
static uint32_t propertyTagIndex = 0;

MailboxPropertyInterface::MailboxPropertyInterface(Mailbox* mailbox) : mailbox(mailbox) {
    propertyTagBuffer[0] = 12; // size
    propertyTagBuffer[1] = 0; // request
    propertyTagIndex = 2;
    propertyTagBuffer[propertyTagIndex] = 0; // null to end
}

void MailboxPropertyInterface::addTag(MailboxPropertyTag tag, ...) {
    va_list args;
    va_start(args, tag);

    propertyTagBuffer[propertyTagIndex++] = tag;

    // Different tags have different requirements, but they tend to fit in
    // groups that behave the same, so we can use handy case fall-throughs.
    switch (tag) {
        case TAG_GET_FIRMWARE_VERSION:
        case TAG_GET_BOARD_MODEL:
        case TAG_GET_BOARD_REVISION:
        case TAG_GET_BOARD_MAC_ADDRESS:
        case TAG_GET_BOARD_SERIAL:
        case TAG_GET_ARM_MEMORY:
        case TAG_GET_VC_MEMORY:
        case TAG_GET_DMA_CHANNELS:
            // 8-byte buffer for response
            propertyTagBuffer[propertyTagIndex++] = 8;
            propertyTagBuffer[propertyTagIndex++] = 0; // request
            propertyTagIndex += 2;
            break;

        case TAG_GET_CLOCKS:
        case TAG_GET_COMMAND_LINE:
            // 256-byte buffer
            propertyTagBuffer[propertyTagIndex++] = 256;
            propertyTagBuffer[propertyTagIndex++] = 0; // request
            propertyTagIndex += 256 >> 2;
            break;

        case TAG_ALLOCATE_BUFFER:
            propertyTagBuffer[propertyTagIndex++] = 8;
            propertyTagBuffer[propertyTagIndex++] = 0; // request
            propertyTagBuffer[propertyTagIndex++] = va_arg(args, uint32_t);
            propertyTagIndex += 1;
            break;

        case TAG_GET_PHYSICAL_SIZE:
        case TAG_SET_PHYSICAL_SIZE:
        case TAG_TEST_PHYSICAL_SIZE:
        case TAG_GET_VIRTUAL_SIZE:
        case TAG_SET_VIRTUAL_SIZE:
        case TAG_TEST_VIRTUAL_SIZE:
        case TAG_GET_VIRTUAL_OFFSET:
        case TAG_SET_VIRTUAL_OFFSET:
            propertyTagBuffer[propertyTagIndex++] = 8;
            propertyTagBuffer[propertyTagIndex++] = 0; // request

            if ((tag == TAG_SET_PHYSICAL_SIZE) || (tag == TAG_SET_VIRTUAL_SIZE) ||
                    (tag == TAG_SET_VIRTUAL_OFFSET) || (tag == TAG_TEST_PHYSICAL_SIZE) ||
                    (tag == TAG_TEST_VIRTUAL_SIZE)) {
                propertyTagBuffer[propertyTagIndex++] = va_arg(args, uint32_t); // width
                propertyTagBuffer[propertyTagIndex++] = va_arg(args, uint32_t); // height
            } else {
                propertyTagIndex += 2;
            }
            break;

        case TAG_GET_ALPHA_MODE:
        case TAG_SET_ALPHA_MODE:
        case TAG_GET_DEPTH:
        case TAG_SET_DEPTH:
        case TAG_GET_PIXEL_ORDER:
        case TAG_SET_PIXEL_ORDER:
        case TAG_GET_PITCH:
            propertyTagBuffer[propertyTagIndex++] = 4;
            propertyTagBuffer[propertyTagIndex++] = 0; // request

            if (tag == TAG_SET_DEPTH || tag == TAG_SET_PIXEL_ORDER ||
                    tag == TAG_SET_ALPHA_MODE) {
                propertyTagBuffer[propertyTagIndex++] = va_arg(args, uint32_t);
            } else {
                propertyTagIndex++;
            }
            break;

        case TAG_GET_OVERSCAN:
        case TAG_SET_OVERSCAN:
            propertyTagBuffer[propertyTagIndex++] = 16;
            propertyTagBuffer[propertyTagIndex++] = 0; // request

            if (tag == TAG_SET_OVERSCAN) {
                propertyTagBuffer[propertyTagIndex++] = va_arg(args, uint32_t); // top
                propertyTagBuffer[propertyTagIndex++] = va_arg(args, uint32_t); // bottom
                propertyTagBuffer[propertyTagIndex++] = va_arg(args, uint32_t); // left
                propertyTagBuffer[propertyTagIndex++] = va_arg(args, uint32_t); // right
            } else {
                propertyTagIndex += 4;
            }
            break;

        default:
            propertyTagIndex--;
            break;
    }

    // null terminate
    propertyTagBuffer[propertyTagIndex] = 0;

    va_end(args);
}

bool MailboxPropertyInterface::process() {
    // set buffer size
    propertyTagBuffer[0] = (propertyTagIndex + 1) * 4;
    propertyTagBuffer[1] = 0; // request

    mailbox->write(MailboxChannels::TAGS_ARM_TO_VC, ((uint32_t)propertyTagBuffer) >> 4);

    if (mailbox->read(MailboxChannels::TAGS_ARM_TO_VC) && propertyTagBuffer[1] == 0x80000000) {
        return true;
    } else {
        return false;
    }
}

bool MailboxPropertyInterface::getProperty(MailboxPropertyTag tag, MailboxProperty& property) {
    property.tag = tag;
    uint32_t idx = 2;
    const uint32_t entryCount = propertyTagBuffer[0] >> 2;

    while (idx < entryCount) {
        if (propertyTagBuffer[idx] == tag) {
            uint32_t* tags = &propertyTagBuffer[idx];
            property.length = tags[2] & 0xFFFF;
            memcpy(property.data.byteBuffer, &tags[3], property.length);

            return true;
        }

        idx += (propertyTagBuffer[idx+1] >> 2) + 3;
    }

    return false;
}
