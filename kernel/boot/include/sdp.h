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

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "uart.h"
#include "atag.h"

enum LogLevel : uint8_t {
    LEVEL_FATAL = 0,
    LEVEL_ERROR,
    LEVEL_WARNING,
    LEVEL_INFO,
    LEVEL_DEBUG
};

enum MessageType : uint8_t {
    MESSAGE_PING = 0,
    MESSAGE_PING_RESPONSE,
    MESSAGE_GET_VERSION,
    MESSAGE_VERSION_RESPONSE,
    MESSAGE_LOG,
    MESSAGE_REQUEST_KERNEL,
    MESSAGE_FILE_INFO,
    MESSAGE_FILE_DATA,
    MESSAGE_ERROR,
    MESSAGE_FILE_DATA_ACK
};

class BootloaderSdp {
private:
    Uart& uart;
    ATags& atags;
    bool connected;

    bool log(LogLevel level, const char* module, const char* message);
    bool ping();
    bool checkVersion();
    bool getKernel();

    void rest();

    void startMessage(MessageType type);
    inline void send(char letter) { send((uint8_t)letter); }
    void send(uint8_t datum);
    void send(uint16_t datum);
    void send(uint32_t datum);
    void send(uint8_t* data, uint16_t length);
    inline void send(const char* string, uint16_t length) { send((uint8_t*)string, length); }

    bool getMessageStart(MessageType& type);
    uint8_t get8();
    uint16_t get16();
    uint32_t get32();
    void get(uint8_t* data, uint16_t length);

    void startHash(uint16_t& crc, MessageType type);
    inline void addHash(uint16_t& crc, char letter) { addHash(crc, (uint8_t)letter); }
    void addHash(uint16_t& crc, uint8_t datum);
    void addHash(uint16_t& crc, uint16_t datum);
    void addHash(uint16_t& crc, uint32_t datum);
    void addHash(uint16_t& crc, uint8_t* data, uint16_t length);
    inline void addHash(uint16_t& crc, const char* string, uint16_t length) { addHash(crc, (uint8_t*)string, length); }

public:
    BootloaderSdp(Uart& uart, ATags& atags);
    void run();
};
