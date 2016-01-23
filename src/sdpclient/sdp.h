// Copyright (C) 2016 Kulshan Concepts. All rights reserved.
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <pthread.h>
#include "serial.h"
#include "logger.h"

class SdpClient {
private:
    Serial& serial;
    Logger& logger;
    pthread_t thread;
    pthread_mutex_t mutex;
    volatile bool stop;

    static void* startThread(void* arg);
    void run();

public:
    SdpClient(Serial& serial, Logger& logger);
    ~SdpClient();

    void waitForKernel();
};
