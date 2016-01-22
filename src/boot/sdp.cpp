// Copyright (c) 2016 Kulshan Concepts. All rights reserved.

#include "sdp.h"

#define MODULE_BOOTLOADER "BootLoader"

#define SDP_VERSION 1

BootloaderSdp::BootloaderSdp(Uart& uart, ATags& atags)
        : uart(uart), atags(atags), connected(false) {
    // nothing else now
}

void BootloaderSdp::run() {
    while (true) {
        connected = false;

        while (!connected) {
            if (ping()) {
                connected = true;
            }
            rest();
        }

        log(LEVEL_INFO, MODULE_BOOTLOADER, "Connected to SDP client.");

        if (!checkVersion()) {
            continue;
        }

        log(LEVEL_INFO, MODULE_BOOTLOADER,
            "SDP versions are compatible. Requesting kernel.");

        if (!getKernel()) {
            continue;
        }

        // if we get here, we're starting over
    }
}

void BootloaderSdp::rest() {
    uint32_t count = 150;
    asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
		: : [count]"r"(count) : "cc");
}

bool BootloaderSdp::ping() {
    sendMessage(MESSAGE_PING, 0, NULL);
    MessageType type;
    if (!getMessageStart(type)) {
        return false;
    }

    return false;
}
