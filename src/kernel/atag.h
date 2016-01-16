// Copyright (C) 2016 Kulshan Concepts. All rights reserved.

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

