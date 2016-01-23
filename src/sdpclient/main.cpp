// Copyright (c) 2016 Kulshan Concepts. All rights reserved.

#include <stdio.h>
#include <string>
#include "sdp.h"
#include "serial.h"

void usage(const char* name) {
    fprintf(stderr, "usage:\n");
    fprintf(stderr, " %s <device>\n", name);
    fprintf(stderr, "\n");
    fprintf(stderr, " device: A serial device connected to the Raspberry Pi\n");
}

int main(int argc, const char** argv) {
    if (argc < 2) {
        usage(argv[0]);
        return 1;
    }

    try {

        std::string device = argv[1];

        printf("Connecting to %s...\n", device.c_str());

        Serial serial(device);

        printf("Waiting for bootloader...\n");

        SdpClient sdpClient(serial);

        // TODO: stuff... want to run SdpClient on a differne thread so commands can be typed here

    } catch (SerialException& ex) {
        fprintf(stderr, "Error: %s\n", ex.getMessage().c_str());
        return 1;
    }

    return 0;
}
