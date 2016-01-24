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
    volatile bool running;

    static void* startThread(void* arg);
    void run();

    void readExactly(void* data, size_t length);

public:
    SdpClient(Serial& serial, Logger& logger);
    ~SdpClient();

    inline bool isRunning() const { return running; }
};
