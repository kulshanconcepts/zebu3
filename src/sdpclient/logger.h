// Copyright (c) 2016 Kulshan Concepts. All rights reserved.
#pragma once

#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <string>
#include "pthread.h"

enum LogLevel : uint8_t {
    LOGLEVEL_FATAL = 0,
    LOGLEVEL_ERROR,
    LOGLEVEL_WARNING,
    LOGLEVEL_INFO,
    LOGLEVEL_DEBUG
};

class Logger {
private:
    pthread_mutex_t mutex;
    static std::string getLevelString(LogLevel level);
    void log(LogLevel level, const std::string& module, const std::string& format, va_list args);

public:
    Logger();
    ~Logger();

    void log(LogLevel level, const std::string& module, const std::string& format, ...);
    void fatal(const std::string& module, const std::string& format, ...);
    void error(const std::string& module, const std::string& format, ...);
    void warning(const std::string& module, const std::string& format, ...);
    void info(const std::string& module, const std::string& format, ...);
    void debug(const std::string& module, const std::string& format, ...);
};
