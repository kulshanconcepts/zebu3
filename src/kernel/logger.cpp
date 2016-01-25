// Copyright (C) 2016 Kulshan Concepts. All rights reserved.

#include "logger.h"
#include "print.h"

Logger* Logger::instance = nullptr;

Logger::Logger(SdpServer& sdpServer)
        : sdpServer(sdpServer) {
    instance = this;
}

void Logger::log(LogLevel level, const char* module, const char* format, va_list args) {
    char buffer[512];
    vsnprintf(buffer, 512, format, args);
    sdpServer.log(level, module, buffer);
}

void Logger::log(LogLevel level, const char* module, const char* format, ...) {
    va_list args;
    va_start(args, format);
    log(level, module, format, args);
    va_end(args);
}

void Logger::fatal(const char* module, const char* format, ...) {
    va_list args;
    va_start(args, format);
    log(LOGLEVEL_FATAL, module, format, args);
    va_end(args);
}

void Logger::error(const char* module, const char* format, ...) {
    va_list args;
    va_start(args, format);
    log(LOGLEVEL_ERROR, module, format, args);
    va_end(args);
}

void Logger::warning(const char* module, const char* format, ...) {
    va_list args;
    va_start(args, format);
    log(LOGLEVEL_WARNING, module, format, args);
    va_end(args);
}

void Logger::info(const char* module, const char* format, ...) {
    va_list args;
    va_start(args, format);
    log(LOGLEVEL_INFO, module, format, args);
    va_end(args);
}

void Logger::debug(const char* module, const char* format, ...) {
    va_list args;
    va_start(args, format);
    log(LOGLEVEL_DEBUG, module, format, args);
    va_end(args);
}
