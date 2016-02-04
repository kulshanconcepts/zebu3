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

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <list>
#include "sdp.h"
#include "serial.h"
#include "logger.h"
#include "version.h"

void usage(const char* name) {
    fprintf(stderr, PRODUCT_NAME " SDP Client " PRODUCT_VERSION_STR "\n" );
    fprintf(stderr, "usage:\n");
    fprintf(stderr, " %s <options> <device>\n", name);
    fprintf(stderr, "\n");
    fprintf(stderr, "options:\n");
    fprintf(stderr, " -k <filename>: Specify kernel file name (default kernel.img)\n");
    fprintf(stderr, "\n");
    fprintf(stderr, " -l #: Set maximum log level to one of:\n");
    fprintf(stderr, "        0: Fatal\n");
    fprintf(stderr, "        1: Error\n");
    fprintf(stderr, "        2: Warning (default)\n");
    fprintf(stderr, "        3: Info\n");
    fprintf(stderr, "        4: Debug\n");
    fprintf(stderr, "\n");
    fprintf(stderr, " -m <module>: Ignore a specified module. Can be repeated.\n");
    fprintf(stderr, "device: A serial device connected to the Raspberry Pi\n");
}

#define MODULE "Client"

bool exitPlease = false;
int maxLog = LOGLEVEL_INFO;

void sigHandler(int sig) {
    (void)sig;
    exitPlease = true;
}

int main(int argc, const char** argv) {
    if (argc < 2) {
        usage(argv[0]);
        return 1;
    }

    std::string device;
    std::string kernelFile = "kernel.img";
    Logger logger;

    int argIndex = 1;
    while (argIndex < argc) {
        const char* arg = argv[argIndex];

        if (strcmp("-l", arg) == 0) {
            argIndex++;
            if (argIndex >= argc) {
                usage(argv[0]);
                return 1;
            }
            arg = argv[argIndex];
            sscanf(arg, "%d", &maxLog);
            if (maxLog > 4) {
                usage(argv[0]);
                return 1;
            }
        } else if (strcmp("-k", arg) == 0) {
            argIndex++;
            if (argIndex >= argc) {
                usage(argv[0]);
                return 1;
            }
            arg = argv[argIndex];
            kernelFile = arg;
        } else if (strcmp("-m", arg) == 0) {
            argIndex++;
            if (argIndex >= argc) {
                usage(argv[0]);
                return 1;
            }
            arg = argv[argIndex];
            logger.ignoreModule(arg);
        } else if (device.empty()) {
            device = arg;
        } else {
            usage(argv[0]);
            return 1;
        }

        argIndex++;
    }

    if (device.empty()) {
        usage(argv[0]);
        return 1;
    }

    logger.setMaxLevel((LogLevel)maxLog);

    try {
        logger.info(MODULE, "Connecting to %s", device.c_str());

        Serial serial(device, logger);

        logger.info(MODULE, "Waiting for bootloader");

        SdpClient sdpClient(serial, logger, kernelFile);

        struct sigaction sigIntHandler;
        sigIntHandler.sa_handler = sigHandler;
        sigemptyset(&sigIntHandler.sa_mask);
        sigIntHandler.sa_flags = 0;
        sigaction(SIGINT, &sigIntHandler, nullptr);

        // TODO: wait for user to type things here in case we need commands
        while (!exitPlease && sdpClient.isRunning()) {
            sleep(1);
        }

        serial.abort();

    } catch (SerialException& ex) {
        logger.fatal(MODULE, "Error: %s", ex.getMessage().c_str());
        return 1;
    }

    return 0;
}
