// Copyright (c) 2016 Kulshan Concepts. All rights reserved.
#pragma once

#include <string>
#include "logger.h"

class SerialException {
private:
    std::string message;
public:
    SerialException(const std::string& message) : message(message) {}
    inline std::string& getMessage() { return message; }
};

class Serial {
private:
    int fd;
    Logger& logger;

public:
    Serial(const std::string& device, Logger& logger);
    ~Serial();

    bool isOpen() const;

    void write(const char* data, size_t length);
    size_t read(char* data, size_t length);
};
