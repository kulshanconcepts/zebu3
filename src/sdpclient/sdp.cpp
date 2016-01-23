// Copyright (c) 2016 Kulshan Concepts. All rights reserved.

#include "sdp.h"

SdpClient::SdpClient(Serial& serial) : serial(serial), stop(false) {
    //pthread_create
}

SdpClient::~SdpClient() {
    stop = true;
    void* ret = nullptr;
    pthread_join(thread, &ret);
}

void* SdpClient::startThread(void* arg) {
    ((SdpClient*)arg)->run();
    return nullptr;
}

void SdpClient::run() {

}
