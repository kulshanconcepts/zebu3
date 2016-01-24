// Copyright (c) 2016 Kulshan Concepts. All rights reserved.

#include <string.h>
#include <unistd.h>
#include "sdp.h"

#define MODULE "SdpClient"

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
    enum {
        waitingForConnection,
        readCommandStart,
    } currentState = waitingForConnection;

    try {
        while (!stop) {
            switch (currentState) {
                case waitingForConnection:
                    serial.waitForData();
                    currentState = readCommandStart;
                    break;
                case readCommandStart:
                    char signature[4];
                    readExactly(signature, 1);
                    if (signature[0] != 'Z') {
                        logger.debug(MODULE, "Not at command start, will keep looking (got %02X)", signature[0]);
                        currentState = waitingForConnection;
                        continue;
                    }
                    readExactly(&signature[1], 3);
                    if (memcmp("Zebu", signature, 4) != 0) {
                        logger.warning(MODULE, "Expected protocol message signature, but got %02X %02X %02X %02X", signature[0], signature[1], signature[2], signature[3]);
                        currentState = waitingForConnection;
                        continue;
                    }
                    uint8_t type;
                    readExactly(&type, 1);
                    logger.debug(MODULE, "Got message type %02X", (unsigned int)type);
                    if (type >= MAX_MESSAGE) {
                        logger.error(MODULE, "Received unknown message %02X", (unsigned int)type);
                        currentState = waitingForConnection;
                        continue;
                    }

                    // TODO: process message
                    logger.warning(MODULE, "Message parsing isn't implemented!");
                    currentState = waitingForConnection;
                    continue;

                default:
                    sleep(1);
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
