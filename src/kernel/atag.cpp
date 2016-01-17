// Copyright (C) 2016 Kulshan Concepts. All rights reserved.

#include "atag.h"
#include "print.h"

ATags::ATags(uint32_t address)
	: address(address), offset(0) {
}

void ATags::reset() {
	offset = 0;
}

ATagDescriptor* ATags::getNextTag() {
	ATagDescriptor* descriptor = (ATagDescriptor*)(address + offset);

	if (descriptor->tag == ATagTypes::ATAG_NONE) {
		return NULL;
	}

	offset += descriptor->size << 2;

	return descriptor;
}

