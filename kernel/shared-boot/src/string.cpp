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

#include "string.h"

size_t strlen(const char* str) {
	size_t ret = 0;
	while (str[ret] != 0) {
		ret++;
	}
	return ret;
}

extern "C" void* memcpy(void* destination, const void* source, size_t count) {
	// TODO: this can be made a lot faster
	char* d = (char*)destination;
	char* s = (char*)source;

	while (count > 0) {
		*d = *s;
		d++;
		s++;
		count--;
	}

	return destination;
}

void itoa(int i, char* buffer) {
	if (i == 0) {
		*buffer = '0';
		buffer++;
		*buffer = 0;
		return;
	}

	if (i < 0) {
		*buffer = '-';
		buffer++;
		i = -i;
	}

	char tmp[12] = {0};
	int o = 10;

	while (i != 0) {
		tmp[o] = '0' + (i % 10);
		o--;
		i /= 10;
	}

	o++;

	while (tmp[o]) {
		*buffer = tmp[o];
		buffer++;
		o++;
	}
}

void itoa_hex(uint32_t i, char* buffer) {
	char digits[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

	if (i == 0) {
		*buffer = '0';
		buffer++;
		*buffer = 0;
		return;
	}

	char tmp[12] = {0};
	int o = 10;

	while (i != 0) {
		tmp[o] = digits[i % 16];
		o--;
		i /= 16;
	}

	o++;

	while (tmp[o]) {
		*buffer = tmp[o];
		buffer++;
		o++;
	}
}
