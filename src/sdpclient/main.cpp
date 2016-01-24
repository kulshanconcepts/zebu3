// Copyright (c) 2016 Kulshan Concepts. All rights reserved.

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string>
#include "sdp.h"
#include "serial.h"
#include "logger.h"

void usage(const char* name) {
    fprintf(stderr, "usage:\n");
    fprintf(stderr, " %s <device>\n", name);
    fprintf(stderr, "\n");
    fprintf(stderr, " device: A serial device connected to the Raspberry Pi\n");
}

#define MODULE "Client"

bool exitPlease = false;

void sigHandler(int sig) {
    (void)sig;
    exitPlease = true;
}

int main(int argc, const char** argv) {
    if (argc < 2) {
        usage(argv[0]);
        return 1;
    }

    Logger logger;

    try {

        std::string device = argv[1];

        logger.info(MODULE, "Connecting to %s", device.c_str());

        Serial serial(device, logger);

        logger.info(MODULE, "Waiting for bootloader");

        SdpClient sdpClient(serial, logger);

        struct sigaction sigIntHandler;
        sigIntHandler.sa_handler = sigHandler;
        sigemptyset(&sigIntHandler.sa_mask);
        sigIntHandler.sa_flags = 0;
        sigaction(SIGINT, &sigIntHandler, nullptr);

        // TODO: wait for user to type things here in case we need commands
        while (!exitPlease && sdpClient.isRunning()) {
            sleep(1);
        }

        serial.abort();

    } catch (SerialException& ex) {
        logger.fatal(MODULE, "Error: %s", ex.getMessage().c_str());
        return 1;
    }

    return 0;
}
