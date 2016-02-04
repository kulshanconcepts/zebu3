/* Copyright (c) 2016, Kulshan Concepts
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. Neither the name of the copyright holder nor the names of its
 *      contributors may be used to endorse or promote products derived from
 *      this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once

#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <string>
#include <list>
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
    std::list<std::string> ignoredModules;

    LogLevel maxLevel;

public:
    Logger();
    ~Logger();

    inline void setMaxLevel(LogLevel level) { maxLevel = level; };

    inline void ignoreModule(const std::string& module) { ignoredModules.push_back(module); }

    void log(LogLevel level, const std::string& module, const std::string& format, ...);
    void fatal(const std::string& module, const std::string& format, ...);
    void error(const std::string& module, const std::string& format, ...);
    void warning(const std::string& module, const std::string& format, ...);
    void info(const std::string& module, const std::string& format, ...);
    void debug(const std::string& module, const std::string& format, ...);
};
