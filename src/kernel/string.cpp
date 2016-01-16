// Copyright (c) 2016 Kulshan Concepts. All rights reserved.

#include "string.h"

size_t strlen(const char* str) {
	size_t ret = 0;
	while (str[ret] != 0) {
		ret++;
	}
	return ret;
}
