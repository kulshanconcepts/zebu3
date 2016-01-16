// Copyright (C) 2016 Kulshan Concepts. All rights reserved.

// Provides basic support functions needed by C/C++

#include <stddef.h>

extern "C" void abort() {
	// TODO: do something better
	while (1) {}
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

