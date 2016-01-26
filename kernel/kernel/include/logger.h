// Copyright (C) 2016 Kulshan Concepts. All rights reserved.
#pragma once

class Logger;

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include "sdp.h"

enum LogLevel : uint8_t {
    LOGLEVEL_FATAL = 0,
    LOGLEVEL_ERROR,
    LOGLEVEL_WARNING,
    LOGLEVEL_INFO,
    LOGLEVEL_DEBUG
};

class Logger {
private:
    static Logger* instance;
    SdpServer& sdpServer;

    void log(LogLevel level, const char* module, const char* format, va_list args);

public:
    Logger(SdpServer& sdpServer);

    void log(LogLevel level, const char* module, const char* format, ...);
    void fatal(const char* module, const char* format, ...);
    void error(const char* module, const char* format, ...);
    void warning(const char* module, const char* format, ...);
    void info(const char* module, const char* format, ...);
    void debug(const char* module, const char* format, ...);
};
