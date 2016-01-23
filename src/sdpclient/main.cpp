// Copyright (c) 2016 Kulshan Concepts. All rights reserved.

#include <stdio.h>
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

        // TODO: stuff... want to run SdpClient on a different thread so commands can be typed here

    } catch (SerialException& ex) {
        logger.fatal(MODULE, "Error: %s", ex.getMessage().c_str());
        return 1;
    }

    return 0;
}
