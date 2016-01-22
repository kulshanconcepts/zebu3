// Copyright (c) 2016 Kulshan Concepts. All rights reserved.

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
    MESSAGE_ERROR
};

class BootloaderSdp {
private:
    Uart& uart;
    ATags& atags;
    bool connected;

    bool log(LogLevel level, const char* module, const char* message) const;
    bool ping();
    bool checkVersion();
    bool getKernel();

    void rest();

    void sendMessage(MessageType type, size_t dataLength, uint8_t* data);
    void sendString(uint16_t length, uint8_t* data);
    bool getMessageStart(MessageType& type);
    uint16_t hashMessage(MessageType type, size_t dataLength, uint8_t* data);

public:
    BootloaderSdp(Uart& uart, ATags& atags);
    void run();
};
