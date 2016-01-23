// Copyright (c) 2016 Kulshan Concepts. All rights reserved.

#include "sdp.h"

#define MODULE "SdpClient"

SdpClient::SdpClient(Serial& serial, Logger& logger) : serial(serial), logger(logger), stop(false) {
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
    while (!stop) {

    }
}
