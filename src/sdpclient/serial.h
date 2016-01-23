// Copyright (c) 2016 Kulshan Concepts. All rights reserved.
#pragma once

#include <string>

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

public:
    Serial(const std::string& device);
    ~Serial();

    bool isOpen() const;

    void write(const char* data, size_t length);
    size_t read(char* data, size_t length);
};
