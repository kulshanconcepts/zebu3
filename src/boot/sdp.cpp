// Copyright (c) 2016 Kulshan Concepts. All rights reserved.

#include "sdp.h"
#include "string.h"

#define MODULE_BOOTLOADER "BootLoader"

#define SDP_VERSION 1

extern "C"
void memset(void* address, uint32_t value, size_t count) {
	uint8_t* addr = (uint8_t*)address;

	while (count >= 4) {
		*((uint32_t*)addr) = value;
		addr += 4;
		count -= 4;
	}

	while (count > 0) {
		*addr = (uint8_t)value;
		addr++;
		count--;
	}
}

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

        log(LEVEL_INFO, MODULE_BOOTLOADER,
            "Kernel is loaded, executing it now.");

        return;
    }
}

void BootloaderSdp::rest() {
    uint32_t count = 150;
    asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
		: : [count]"r"(count) : "cc");
}

bool BootloaderSdp::ping() {
    startMessage(MESSAGE_PING);
    uint16_t hash;
    startHash(hash, MESSAGE_PING);
    send(hash);

    MessageType type;
    if (!getMessageStart(type)) {
        return false;
    }

    if (type != MESSAGE_PING_RESPONSE) {
        return false;
    }

    startHash(hash, MESSAGE_PING_RESPONSE);
    if (get16() != hash) {
        return false;
    }

    return true;
}

bool BootloaderSdp::log(LogLevel level, const char* module, const char* message) {
    uint16_t hash;

    startMessage(MESSAGE_LOG);
    startHash(hash, MESSAGE_LOG);

    send((uint8_t)level);
    addHash(hash, (uint8_t)level);

    uint16_t len = strlen(module);
    send(module, len);
    addHash(hash, module, len);

    len = strlen(message);
    send(message, len);
    addHash(hash, message, len);

    send(hash);

    return true;
}

bool BootloaderSdp::checkVersion() {
    uint16_t hash;

    startMessage(MESSAGE_GET_VERSION);
    startHash(hash, MESSAGE_GET_VERSION);

    send((uint16_t)SDP_VERSION);
    addHash(hash, (uint16_t)SDP_VERSION);

    send(hash);

    MessageType type;
    if (!getMessageStart(type)) {
        return false;
    }

    if (type != MESSAGE_VERSION_RESPONSE) {
        return false;
    }

    startHash(hash, MESSAGE_VERSION_RESPONSE);

    uint16_t version = get16();
    addHash(hash, version);

    if (get16() != hash) {
        return false;
    }

    if (SDP_VERSION != version) {
        return false;
    }

    return true;
}

bool BootloaderSdp::getKernel() {
    startMessage(MESSAGE_REQUEST_KERNEL);
    uint16_t hash;
    startHash(hash, MESSAGE_REQUEST_KERNEL);
    send(hash);

    MessageType type;
    if (!getMessageStart(type)) {
        return false;
    }

    if (type != MESSAGE_FILE_INFO) {
        return false;
    }

    startHash(hash, MESSAGE_FILE_INFO);

    uint16_t len = get16();
    char name[256] = {0};
    get((uint8_t*)name, len);
    addHash(hash, name, len);

    uint32_t size = get32();
    addHash(hash, size);

    if (get16() != hash) {
        return false;
    }

    uint8_t* kernel = (uint8_t*)0x8000;

    // Now expect some MESSAGE_FILE_DATA messages!
    while (size > 0) {
        if (!getMessageStart(type)) {
            return false;
        }

        if (type != MESSAGE_FILE_DATA) {
            return false;
        }

        startHash(hash, MESSAGE_FILE_DATA);

        uint16_t len = get16();
        get(kernel, len);
        addHash(hash, kernel, len);

        if (get16() != hash) {
            return false;
        }

        kernel += len;
        size -= len;
    }

    return true;
}

/* HASHERS */

void BootloaderSdp::startHash(uint16_t& crc, MessageType type) {
    crc = 0xFFFF;
    addHash(crc, 'Z');
    addHash(crc, 'e');
    addHash(crc, 'b');
    addHash(crc, 'u');
    addHash(crc, (uint8_t)type);
}

void BootloaderSdp::addHash(uint16_t& crc, uint32_t datum) {
    size_t d = 4;
    while (d--) {
        addHash(crc, ((uint8_t*)&datum)[d]);
    }
}

void BootloaderSdp::addHash(uint16_t& crc, uint16_t datum) {
    size_t d = 2;
    while (d--) {
        addHash(crc, ((uint8_t*)&datum)[d]);
    }
}

void BootloaderSdp::addHash(uint16_t& crc, uint8_t datum) {
    uint16_t x = crc >> 8 ^ datum;
    x ^= x >> 4;
    crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x << 5)) ^ ((uint16_t)x);
}

void BootloaderSdp::addHash(uint16_t& crc, uint8_t* data, uint16_t length) {
    addHash(crc, length);
    while (length--) {
        addHash(crc, *data++);
    }
}

/* SENDERS */

void BootloaderSdp::send(uint8_t* data, uint16_t length) {
    uart.write((char*)&length, sizeof(length));;
    uart.write((char*)data, length);
}

void BootloaderSdp::send(uint8_t datum) {
    uart.putc(datum);
}

void BootloaderSdp::send(uint16_t datum) {
    uart.write((char*)&datum, sizeof(datum));
}

void BootloaderSdp::send(uint32_t datum) {
    uart.write((char*)&datum, sizeof(datum));
}

void BootloaderSdp::startMessage(MessageType type) {
    uart.write("Zebu", 4);
    uart.putc(type);
}

/* GETTERS */

bool BootloaderSdp::getMessageStart(MessageType& type) {
    while (true) {
        uint8_t b = uart.getc();
        if (b != 'Z') {
            return false;
        }

        b = uart.getc();

        if (b != 'e') {
            return false;
        }

        b = uart.getc();

        if (b != 'b') {
            return false;
        }

        b = uart.getc();

        if (b != 'u') {
            return false;
        }

        type = (MessageType) uart.getc();

        return true;
    }
}

uint8_t BootloaderSdp::get8() {
    return uart.getc();
}

uint16_t BootloaderSdp::get16() {
    return (uart.getc() << 8) | uart.getc();
}

uint32_t BootloaderSdp::get32() {
    return (uart.getc() << 24) | (uart.getc() << 16) | (uart.getc() << 8) | uart.getc();
}

void BootloaderSdp::get(uint8_t* data, uint16_t length) {
    while (length--) {
        *data++ = uart.getc();
    }
}
