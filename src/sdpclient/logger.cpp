// Copyright (c) 2016 Kulshan Concepts. All rights reserved.

#include <stdarg.h>
#include <stdio.h>
#include "logger.h"

Logger::Logger() : mutex(PTHREAD_MUTEX_INITIALIZER) {

}

Logger::~Logger() {}

void Logger::log(LogLevel level, const std::string& module, const std::string& format, va_list args) {
    // TODO: check if log level and module match current logging settings
    pthread_mutex_lock(&mutex);
    vprintf(("[" + getLevelString(level) + ":" + module + "]: " + format + "\n").c_str(), args);
    pthread_mutex_unlock(&mutex);
}

void Logger::log(LogLevel level, const std::string& module, const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    log(level, module, format, args);
    va_end(args);
}

void Logger::fatal(const std::string& module, const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    log(LOGLEVEL_FATAL, module, format, args);
    va_end(args);
}

void Logger::error(const std::string& module, const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    log(LOGLEVEL_ERROR, module, format, args);
    va_end(args);
}

void Logger::warning(const std::string& module, const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    log(LOGLEVEL_WARNING, module, format, args);
    va_end(args);
}

void Logger::info(const std::string& module, const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    log(LOGLEVEL_INFO, module, format, args);
    va_end(args);
}

void Logger::debug(const std::string& module, const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    log(LOGLEVEL_DEBUG, module, format, args);
    va_end(args);
}

std::string Logger::getLevelString(LogLevel level) {
    switch (level) {
        case LOGLEVEL_FATAL:
            return "FATAL";
        case LOGLEVEL_ERROR:
            return "ERROR";
        case LOGLEVEL_WARNING:
            return "WARNING";
        case LOGLEVEL_INFO:
            return "INFO";
        case LOGLEVEL_DEBUG:
            return "DEBUG";
        default:
            return "????";
    }
}
