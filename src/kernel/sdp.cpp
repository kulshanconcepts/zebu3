// Copyright (C) 2016 Kulshan Concepts. All rights reserved.

#include "sdp.h"

SdpServer::SdpServer(Uart& uart)
        : uart(uart) {
    //
}

bool SdpServer::connect() {
    // TODO: stuff
    return false;
}

void SdpServer::log(uint8_t level, const char* module, const char* message) {
    // TODO: stuff
}

uint16_t SdpServer::computeHash(MessageType type, const void* data, size_t length) {
    // TODO: stuff
    return 0;
}

bool SdpServer::sendMessage(MessageType type, const void* data, size_t length) {
    // TODO: stuff
    return false;
}
