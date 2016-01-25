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
    bool ping();
    bool checkVersion();

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
    void addHash(uint16_t& crc, const uint8_t* data, uint16_t length);
    inline void addHash(uint16_t& crc, const char* string, uint16_t length) { addHash(crc, (uint8_t*)string, length); }

public:
    SdpServer(Uart& uart);

    bool connect();
    void log(uint8_t level, const char* module, const char* message);
};
