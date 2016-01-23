// Copyright (c) 2016 Kulshan Concepts. All rights reserved.

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include "serial.h"

Serial::Serial(const std::string& device, Logger& logger) : logger(logger) {
    fd = open(device.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);

    if (-1 == fd) {
        throw SerialException("Could not open device");
    }

    if (!isatty(fd)) {
        close(fd);
        throw SerialException("Specified device is not a TTY");
    }

    termios termAttr;
    if (-1 == tcgetattr(fd, &termAttr)) {
        close(fd);
        throw SerialException("Could not get device attributes");
    }

    termAttr.c_cc[VTIME] = 0;
    termAttr.c_cc[VMIN] = 0;

    termAttr.c_iflag = 0;
    termAttr.c_oflag = 0;
    termAttr.c_cflag = CS8 | CREAD | CLOCAL;
    termAttr.c_lflag = 0;

    if (0 > cfsetispeed(&termAttr, B115200) ||
        0 > cfsetospeed(&termAttr, B115200)) {

        close(fd);
        throw SerialException("Could not set device baud rate");
    }

    if (-1 == tcsetattr(fd, TCSAFLUSH, &termAttr)) {
        close(fd);
        throw SerialException("Could not set device attributes");
    }
}

Serial::~Serial() {
    if (-1 != fd) {
        close(fd);
    }
}

bool Serial::isOpen() const {
    return -1 != fd;
}

void Serial::write(const char* data, size_t length) {
    ssize_t bw = 0;
    while ((size_t)bw < length) {
        bw  = ::write(fd, data, length);
        if (-1 == bw) {
            throw SerialException("Could not write data");
        }

        length -= bw;
        data += bw;
    }
}

size_t Serial::read(char* data, size_t length) {
    ssize_t br = ::read(fd, data, length);
    if (br == -1) {
        throw SerialException("Could not read data");
    }
    return br;
}
