// Copyright (c) 2016 Kulshan Concepts. All rights reserved.

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
