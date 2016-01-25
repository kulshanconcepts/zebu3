// Copyright (C) 2016 Kulshan Concepts. All rights reserved.

// Provides basic support functions needed by C/C++

#include <stddef.h>

extern "C" void abort() {
	// TODO: do something better
	while (1) {}
}
