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

#include "sdp.h"
#include "string.h"

#define SDP_VERSION 1

SdpServer::SdpServer(Uart& uart)
        : uart(uart) {
    //
}

bool SdpServer::connect() {
    ping();

    checkVersion();

    return true;
}

bool SdpServer::sendMessage(MessageType type, const void* data, size_t length) {
    uint16_t hash;
    startHash(hash, type);

    uart.putc('Z');
    uart.putc('e');
    uart.putc('b');
    uart.putc('u');
    uart.putc(type);

    if (data != nullptr && length > 0) {
        uart.write((const char*)data, length);
        addHash(hash, (const uint8_t*)data, length);
    }

    uart.write((char*)&hash, sizeof(hash));

    return true;
}

bool SdpServer::ping() {
    if (!sendMessage(MESSAGE_PING, nullptr, 0)) {
        return false;
    }

    MessageType responseType;
    if (!getMessageStart(responseType)) {
        return false;
    }

    if (responseType != MESSAGE_PING_RESPONSE) {
        return false;
    }

    uint16_t hash;
    startHash(hash, MESSAGE_PING_RESPONSE);
    if (get16() != hash) {
        return false;
    }

    return true;
}

bool SdpServer::checkVersion() {
    uint16_t ourVersion = SDP_VERSION;
    if (!sendMessage(MESSAGE_GET_VERSION, &ourVersion, sizeof(ourVersion))) {
        return false;
    }

    MessageType responseType;
    if (!getMessageStart(responseType)) {
        return false;
    }

    if (responseType != MESSAGE_VERSION_RESPONSE) {
        return false;
    }

    uint16_t hash;
    startHash(hash, MESSAGE_VERSION_RESPONSE);

    uint16_t theirVersion = get16();
    addHash(hash, theirVersion);

    if (get16() != hash) {
        return false;
    }

    if (theirVersion != SDP_VERSION) {
        return false;
    }

    return true;
}

void SdpServer::log(uint8_t level, const char* module, const char* message) {
    const size_t bufferLength = 576;
    char buffer[bufferLength];

    buffer[0] = level;
    uint16_t modLen = strlen(module);
    memcpy(&buffer[1], &modLen, sizeof(modLen));
    memcpy(&buffer[3], module, modLen);

    uint16_t msgLen = strlen(message);
    memcpy(&buffer[3 + modLen], &msgLen, sizeof(msgLen));
    memcpy(&buffer[5 + modLen], message, msgLen);

    sendMessage(MESSAGE_LOG, buffer, 5 + modLen + msgLen);
}

void SdpServer::startHash(uint16_t& crc, MessageType type) {
    crc = 0xFFFF;
    addHash(crc, 'Z');
    addHash(crc, 'e');
    addHash(crc, 'b');
    addHash(crc, 'u');
    addHash(crc, (uint8_t)type);
}

void SdpServer::addHash(uint16_t& crc, uint32_t datum) {
	addHash(crc, (uint8_t)(datum & 0xFF));
	addHash(crc, (uint8_t)(datum >> 8));
	addHash(crc, (uint8_t)(datum >> 16));
    addHash(crc, (uint8_t)(datum >> 24));
}

void SdpServer::addHash(uint16_t& crc, uint16_t datum) {
	addHash(crc, (uint8_t)(datum & 0xFF));
	addHash(crc, (uint8_t)(datum >> 8));
}

void SdpServer::addHash(uint16_t& crc, uint8_t datum) {
    uint16_t x = crc >> 8 ^ datum;
    x ^= x >> 4;
    crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x << 5)) ^ ((uint16_t)x);
}

void SdpServer::addHash(uint16_t& crc, const uint8_t* data, uint16_t length) {
    while (length > 0) {
        addHash(crc, *data);
        data++;
        length--;
    }
}

bool SdpServer::getMessageStart(MessageType& type) {
    while (true) {
        uint8_t b = uart.getc();
        if (b != 'Z') {
            return false;
        }

        b = uart.getc();

        if (b != 'e') {
            return false;
        }

        b = uart.getc();

        if (b != 'b') {
            return false;
        }

        b = uart.getc();

        if (b != 'u') {
            return false;
        }

        type = (MessageType) uart.getc();

        return true;
    }
}

uint8_t SdpServer::get8() {
    return uart.getc();
}

uint16_t SdpServer::get16() {
    return uart.getc() | ((uint16_t)uart.getc() << 8);
}

uint32_t SdpServer::get32() {
    return uart.getc() | ((uint32_t)uart.getc() << 8) | ((uint32_t)uart.getc() << 16) | ((uint32_t)uart.getc() << 24);
}

void SdpServer::get(uint8_t* data, uint16_t length) {
    while (length--) {
        *data++ = uart.getc();
    }
}
