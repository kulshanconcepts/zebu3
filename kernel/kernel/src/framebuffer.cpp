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

#include "framebuffer.h"
#include "mailbox.h"
#include "mailboxproperty.h"
#include "logger.h"

Framebuffer::Framebuffer() : width(1600), height(900), depth(32), pitch(4), buffer(nullptr), offset(0) {
    Mailbox mailbox;
    MailboxPropertyInterface mpi(&mailbox);

    mpi.addTag(MailboxPropertyTag::TAG_ALLOCATE_BUFFER, 0x00000000);
    mpi.addTag(MailboxPropertyTag::TAG_SET_PHYSICAL_SIZE, width, height);
    mpi.addTag(MailboxPropertyTag::TAG_SET_VIRTUAL_SIZE, width, height * 2);
    mpi.addTag(MailboxPropertyTag::TAG_SET_DEPTH, depth);
    mpi.addTag(MailboxPropertyTag::TAG_GET_PITCH);
    mpi.addTag(MailboxPropertyTag::TAG_GET_PHYSICAL_SIZE);
    mpi.addTag(MailboxPropertyTag::TAG_GET_DEPTH);
    mpi.addTag(MailboxPropertyTag::TAG_GET_VIRTUAL_OFFSET);
    mpi.process();

    MailboxProperty property;

    if (mpi.getProperty(TAG_GET_PHYSICAL_SIZE, property)) {
        width = property.data.intBuffer[0];
        height = property.data.intBuffer[1];
    }

    if (mpi.getProperty(TAG_GET_DEPTH, property)) {
        depth = property.data.intBuffer[0];
    }

    if (mpi.getProperty(TAG_GET_PITCH, property)) {
        pitch = property.data.intBuffer[0];
    }

    if (mpi.getProperty(TAG_ALLOCATE_BUFFER, property)) {
        buffer = (uint8_t*)property.data.intBuffer[0];
    }

    if (mpi.getProperty(TAG_GET_VIRTUAL_OFFSET, property)) {
        offset = property.data.intBuffer[0];
    }

    Logger::getInstance()->info("Framebuffer", "Initialized framebuffer at %dx%dx%dbpp (pitch %d) at %X. "
        "Virtual offset is %d.", width, height, depth, pitch, buffer, offset);
}

void Framebuffer::setPixel(uint32_t x, uint32_t y, uint32_t argb) {
    uint8_t* pixel = (uint8_t*)(x * (depth / 8) + y * pitch + (uint32_t)buffer);

    uint8_t red = (uint8_t)((argb & 0x00FF0000) >> 16);
    uint8_t green = (uint8_t)((argb & 0x0000FF00) >> 8);
    uint8_t blue = (uint8_t)((argb & 0x000000FF) >> 0);
    uint8_t alpha = (uint8_t)((argb & 0xFF000000) >> 24);

//    Logger::getInstance()->info("Framebuffer", "Setting pixel (%d,%d) at 0x%X to (%d, %d, %d)",
//        x, y, pixel, red, green, blue);


    if (depth == 32) {
        *(pixel++) = red;
        *(pixel++) = green;
        *(pixel++) = blue;
        *(pixel++) = alpha;
    } else if (depth == 24) {
        *(pixel++) = red;
        *(pixel++) = green;
        *(pixel++) = blue;
    } else if (depth == 16) {
        // it's RGB565, needs to be packed into two bytes
        uint16_t rgb565 = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);
        *(pixel++) = rgb565 & 0xFF; // little endian
        *(pixel++) = rgb565 >> 8;
    } else {
        // probably 8 bit
        // TODO: support this? would need a palette
    }
}
