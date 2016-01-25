// Copyright (C) 2016 Kulshan Concepts. All rights reserved.
#pragma once

class SdpServer;

#include <stddef.h>
#include <stdint.h>
#include "uart.h"
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

class SdpServer {
private:
    Uart& uart;
    bool sendMessage(MessageType type, const void* data, size_t length);
    uint16_t computeHash(MessageType type, const void* data, size_t length);

public:
    SdpServer(Uart& uart);

    bool connect();
    void log(uint8_t level, const char* module, const char* message);
};
