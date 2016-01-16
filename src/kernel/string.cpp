// Copyright (c) 2016 Kulshan Concepts. All rights reserved.

#include "string.h"

size_t strlen(const char* str) {
	size_t ret = 0;
	while (str[ret] != 0) {
		ret++;
	}
	return ret;
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
