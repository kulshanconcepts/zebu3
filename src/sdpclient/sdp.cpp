// Copyright (c) 2016 Kulshan Concepts. All rights reserved.

#include <string.h>
#include <unistd.h>
#include "sdp.h"

#define MODULE "SdpClient"

SdpClient::SdpClient(Serial& serial, Logger& logger) : serial(serial), logger(logger), stop(false), running(true) {
    logger.debug(MODULE, "Starting SDP processing thread");
    pthread_create(&thread, nullptr, startThread, (void*)this);
}

SdpClient::~SdpClient() {
    logger.debug(MODULE, "Deconstructing, waiting for thread to stop");
    stop = true;
    pthread_join(thread, nullptr);
}

void* SdpClient::startThread(void* arg) {
    ((SdpClient*)arg)->run();
    return nullptr;
}

void SdpClient::run() {
    try {
        while (!stop) {
            // TODO: This needs to be done a bit differently so someone can issue a command if they want while we're waiting

            char signature[4];
            readExactly(signature, 1);
            if (signature[0] != 'Z') {
                logger.debug(MODULE, "Not at command start, will keep looking (got %02X)", signature[0]);
                continue;
            }
            readExactly(&signature[1], 3);
            if (memcmp("Zebu", signature, 4) != 0) {
                logger.warning(MODULE, "Expected protocol message signature, but got %02X %02X %02X %02X", signature[0], signature[1], signature[2], signature[3]);
                continue;
            }
            uint8_t type;
            readExactly(&type, 1);
            logger.debug(MODULE, "Got message type %02X", (unsigned int)type);
            if (type >= MAX_MESSAGE) {
                logger.error(MODULE, "Received unknown message %02X", (unsigned int)type);
                continue;
            }

            switch (type) {
                case MESSAGE_PING:
                    processPing();
                    break;
                default:
                    logger.warning(MODULE, "Message parsing isn't implemented for %02X!", (unsigned int)type);
            }
        }
    } catch (SerialException& ex) {
        logger.fatal(MODULE, "Serial connection lost: %s", ex.getMessage().c_str());
    }

    running = false;
}

void SdpClient::readExactly(void* data, size_t length) {
    char* dataPtr = (char*)data;
    while (length > 0) {
        serial.waitForData();
        size_t br = serial.read(dataPtr, length);
        logger.debug(MODULE, "Read %u of %u desired bytes", br, length);
        length -= br;
        dataPtr += br;
    }
}

void SdpClient::processPing() {
    // no parameters expected
    if (verifyRecvHash()) {
        logger.info(MODULE, "Responding to ping from server");
        SdpMessage message = startMessage(MESSAGE_PING_RESPONSE);
        sendMessage(message);
    }
}

SdpMessage SdpClient::startMessage(MessageType type) {
    SdpMessage msg;
    msg.type = type;

    return msg;
}

void SdpClient::sendMessage(const SdpMessage& message) {
    std::vector<uint8_t> data(message.data.size() + 5);
    memcpy(data.data(), "Zebu", 4);
    data.data()[4] = message.type;
    memcpy(&(data.data()[5]), message.data.data(), message.data.size());
    uint16_t hash = computeHash(data.data(), data.size());
    data.push_back((uint8_t)(hash >> 8));
    data.push_back((uint8_t)(hash & 0xFF));

    serial.write((const char*)data.data(), data.size());
}

void SdpClient::resetRecvHash(MessageType type) {
    recvHash = 0xFFFF;
    addRecvHash((uint8_t)'Z');
    addRecvHash((uint8_t)'e');
    addRecvHash((uint8_t)'b');
    addRecvHash((uint8_t)'u');
    addRecvHash((uint8_t)type);
}

void SdpClient::addRecvHash(uint8_t datum) {
    uint16_t x = recvHash >> 8 ^ datum;
    x ^= x >> 4;
    recvHash = (recvHash << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x << 5)) ^ ((uint16_t)x);
}

uint16_t SdpClient::computeHash(void* data, size_t length) {
    uint16_t crc = 0xFFFF;
    uint16_t x;
    uint8_t* dataPtr = (uint8_t*)data;

    while (length-- > 0) {
        x = crc >> 8 ^ *dataPtr++;
        x ^= x >> 4;
        crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x << 5)) ^ ((uint16_t)x);
    }

    return crc;
}

bool SdpClient::verifyRecvHash() {
    uint16_t wire;
    readExactly(&wire, 2);
    bool good = (wire == recvHash);
    if (!good) {
        logger.warning(MODULE, "CRC on received message not as expected. Data corrupt and ignored.");
    }
    return good;
}
