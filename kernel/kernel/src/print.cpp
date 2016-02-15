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

#include "print.h"
#include "string.h"

void vsnprintf(char* buffer, size_t bufferSize, const char* format, va_list args) {
	size_t idx = 0;
	enum {
		NORMAL,
		BEGIN
	} state = NORMAL;

	size_t maxIndex = bufferSize - 1;

	while (*format && idx < maxIndex) {
		switch (state) {
			case NORMAL:
				if (*format == '%') {
					state = BEGIN;
				} else {
					buffer[idx++] = *format;
				}
				break;
			case BEGIN:
				// expecting a type or numbers or whatever
				// right now we only support the type
				if (*format == '%') {
					buffer[idx++] = '%';

					state = NORMAL;
				} else if (*format == 's') {
					char* p = va_arg(args, char*);
					while (*p) {
						buffer[idx++] = *p;
						p++;
						if (idx >= maxIndex) break;
					}

					state = NORMAL;
				} else if (*format == 'd') {
					int i = va_arg(args, int);
					char tmp[12] = {0};
					itoa(i, tmp);
					char* p = &tmp[0];
					while (*p) {
						buffer[idx++] = *p;
						p++;
						if (idx >= maxIndex) break;
					}

					state = NORMAL;
				} else if (*format == 'X' || *format == 'x' || *format == 'p' || *format == 'P') {
					int i = va_arg(args, int);
					char tmp[12] = {0};
					if (*format == 'X' || *format == 'P') {
						itoa_hex(i, tmp);
					} else {
						itoa_hex_lower(i, tmp);
					}

					if (*format == 'P' || *format == 'p') {
						char tmp2[12] = "0x00000000";
						char* p2 = &tmp2[0];
						size_t toCopy = 10 - strlen(tmp);
						while (toCopy > 0) {
							buffer[idx++] = *p2;
							p2++;
							toCopy--;
							if (idx >= maxIndex) {
								tmp[0] = 0; // force the loop below to jump out
								break;
							}
						}
					}

					char* p = &tmp[0];
					while (*p) {
						buffer[idx++] = *p;
						p++;
						if (idx >= maxIndex) break;
					}

					state = NORMAL;
				} else {
					buffer[idx++] = '?';
				}
				break;
		}

		format++;
	}

	buffer[idx] = 0;
}
