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

#include <stddef.h>
#include <stdint.h>

enum ATagTypes : uint32_t {
	ATAG_NONE      = 0x00000000,
	ATAG_CORE      = 0x54410001,
	ATAG_MEM       = 0x54410002,
	ATAG_VIDEOTEXT = 0x54410003,
	ATAG_RAMDISK   = 0x54410004,
	ATAG_INITRD2   = 0x54410005,
	ATAG_SERIAL    = 0x54410006,
	ATAG_REVISION  = 0x54410007,
	ATAG_VIDEOLFB  = 0x54410008,
	ATAG_CMDLINE   = 0x54410009
};

struct ATagCore {
	uint32_t flags;
	uint32_t pageSize;
	uint32_t rootDevice;
};

struct ATagMem {
	uint32_t size;
	uint32_t startAddress;
};

struct ATagDescriptor {
	uint32_t size;
	ATagTypes tag;
	union {
		ATagCore core;
		ATagMem mem;
	};
};

class ATags {
	private:
	uint32_t address;
	uint32_t offset;

	public:
	ATags(uint32_t address);
	void reset();
	ATagDescriptor* getNextTag();
};
