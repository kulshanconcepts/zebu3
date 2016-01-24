// Copyright (C) 2016 Kulshan Concepts. All rights reserved.
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
    pthread_t thread;
    pthread_mutex_t mutex;
    volatile bool stop;
    volatile bool running;
    uint16_t recvHash;

    static void* startThread(void* arg);
    void run();

    void readExactly(void* data, size_t length);

    SdpMessage startMessage(MessageType type);
    void sendMessage(const SdpMessage& message);
    uint16_t computeHash(void* data, size_t length);

    void resetRecvHash(MessageType type);
    void addRecvHash(uint8_t datum);
    void addRecvHash(uint16_t datum);
    void addRecvHash(uint32_t datum);
    void addRecvHash(void* data, size_t length);
    bool verifyRecvHash();

    void processPing();

public:
    SdpClient(Serial& serial, Logger& logger);
    ~SdpClient();

    inline bool isRunning() const { return running; }
};
