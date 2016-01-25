// Copyright (C) 2016 Kulshan Concepts. All rights reserved.
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
				} else if (*format == 'X') {
					int i = va_arg(args, int);
					char tmp[12] = {0};
					itoa_hex(i, tmp);
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
