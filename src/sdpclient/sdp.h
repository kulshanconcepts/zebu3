// Copyright (C) 2016 Kulshan Concepts. All rights reserved.
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <pthread.h>
#include "serial.h"

class SdpClient {
private:
    Serial& serial;
    pthread_t thread;
    volatile bool stop;

    static void* startThread(void* arg);
    void run();

public:
    SdpClient(Serial& serial);
    ~SdpClient();
};
