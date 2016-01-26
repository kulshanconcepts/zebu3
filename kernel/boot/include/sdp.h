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
