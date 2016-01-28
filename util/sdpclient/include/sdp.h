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
#include <pthread.h>
#include <vector>
#include "serial.h"
#include "logger.h"

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
    MESSAGE_FILE_DATA_ACK,
    MAX_MESSAGE
};

struct SdpMessage {
    MessageType type;
    std::vector<uint8_t> data;
};

class SdpClient {
private:
    Serial& serial;
    Logger& logger;
    const std::string kernelFile;
    pthread_t thread;
    pthread_mutex_t mutex;
    volatile bool stop;
    volatile bool running;
    uint16_t recvHash;
    FILE* fp;
    size_t fileSize;

    static void* startThread(void* arg);
    void run();

    void readExactly(void* data, size_t length);

    void sendError(const char* message);

    SdpMessage startMessage(MessageType type) const;
    void appendMessage(SdpMessage& message, uint8_t datum) const;
    void appendMessage(SdpMessage& message, uint16_t datum) const;
    void appendMessage(SdpMessage& message, uint32_t datum) const;
    void appendMessage(SdpMessage& message, const void* data, size_t length) const;
    void sendMessage(const SdpMessage& message);
    uint16_t computeHash(const void* data, size_t length);

    void resetRecvHash(MessageType type);
    void addRecvHash(uint8_t datum);
    void addRecvHash(uint16_t datum);
    void addRecvHash(uint32_t datum);
    void addRecvHash(const void* data, size_t length);
    bool verifyRecvHash();

    void processPing();
    void processGetVersion();
    void processLog();
    void processRequestKernel();
    void processFileDataAck(bool skipReading);

public:
    SdpClient(Serial& serial, Logger& logger, const std::string& kernelFile);
    ~SdpClient();

    inline bool isRunning() const { return running; }
};
