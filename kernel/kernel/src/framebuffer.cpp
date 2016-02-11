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

Framebuffer::Framebuffer() : width(640), height(480), depth(32), pitch(640*4), buffer(nullptr), bufferSize(0), offset(0) {
    Mailbox mailbox;
    MailboxPropertyInterface mpi(&mailbox);

    mpi.addTag(MailboxPropertyTag::TAG_ALLOCATE_BUFFER, 16);
    mpi.addTag(MailboxPropertyTag::TAG_SET_PHYSICAL_SIZE, width, height);
    mpi.addTag(MailboxPropertyTag::TAG_SET_VIRTUAL_SIZE, width, height);
    mpi.addTag(MailboxPropertyTag::TAG_SET_DEPTH, depth);
    mpi.addTag(MailboxPropertyTag::TAG_GET_PITCH);
    mpi.addTag(MailboxPropertyTag::TAG_GET_PHYSICAL_SIZE);
    mpi.addTag(MailboxPropertyTag::TAG_GET_DEPTH);
    mpi.addTag(MailboxPropertyTag::TAG_GET_VIRTUAL_OFFSET);

    if (!mpi.process()) {
        Logger::getInstance()->warning("Framebuffer", "Could not query the GPU.");
        return;
    }

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
        bufferSize = property.data.intBuffer[1];
    }

    if (mpi.getProperty(TAG_GET_VIRTUAL_OFFSET, property)) {
        offset = property.data.intBuffer[0];
    }

    Logger::getInstance()->info("Framebuffer", "Initialized %d-byte framebuffer at %dx%dx%dbpp (pitch %d) at %X. "
        "Virtual offset is %d.", bufferSize, width, height, depth, pitch, buffer, offset);
}

#define PIXEL_POINTER(x, y) (uint8_t*)(x * (depth / 8) + y * pitch + (uint32_t)buffer)

void Framebuffer::setPixel(uint32_t x, uint32_t y, uint32_t argb) {
    uint8_t* pixel = PIXEL_POINTER(x, y);

    uint8_t red = (uint8_t)((argb & 0x00FF0000) >> 16);
    uint8_t green = (uint8_t)((argb & 0x0000FF00) >> 8);
    uint8_t blue = (uint8_t)((argb & 0x000000FF) >> 0);
    uint8_t alpha = (uint8_t)((argb & 0xFF000000) >> 24);

    static int count = 10;
    if (count > 0) {
        Logger::getInstance()->info("Framebuffer", "Setting pixel (%d,%d) at 0x%X to (%d, %d, %d)",
            x, y, pixel, red, green, blue);
        count--;
    }


    if (depth == 32) {
        *pixel = red;
        pixel++;
        *pixel = green;
        pixel++;
        *pixel = blue;
        pixel++;
        *pixel = alpha;
        pixel++;
    } else if (depth == 24) {
        *pixel = red;
        pixel++;
        *pixel = green;
        pixel++;
        *pixel = blue;
        pixel++;
    } else if (depth == 16) {
        // it's RGB565, needs to be packed into two bytes
        uint16_t rgb565 = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);
        *pixel = rgb565 & 0xFF; // little endian
        pixel++;
        *pixel = rgb565 >> 8;
        pixel++;
    } else {
        // probably 8 bit
        // TODO: support this? would need a palette
    }
}

uint32_t Framebuffer::getPixel(uint32_t x, uint32_t y) {
    uint8_t* pixel = PIXEL_POINTER(x, y);

    uint32_t red = 0;
    uint32_t green = 0;
    uint32_t blue = 0;
    uint32_t alpha = 0xFF;

    if (depth == 32) {
        red = *pixel;
        pixel++;
        green = *pixel;
        pixel++;
        blue = *pixel;
        pixel++;
        alpha = *pixel;
        pixel++;
    } else if (depth == 24) {
        red = *pixel;
        pixel++;
        green = *pixel;
        pixel++;
        blue = *pixel;
        pixel++;
    } else if (depth == 16) {
        // TODO: implement 16-bit
    } else {
        // TODO: implement 8-bit
    }

    return (alpha << 24) | (red << 16) | (green << 8) | blue;
}
