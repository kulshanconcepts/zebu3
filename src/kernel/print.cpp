// Copyright (C) 2016 Kulshan Concepts. All rights reserved.
#include <stdarg.h>

#include "print.h"
#include "uart.h"
#include "string.h"

void kprint(const char* message) {
	Uart::getInstance()->puts(message);
}

void kprintf(const char* format, ...) {
	va_list args;
	va_start(args, format);

	const int MAX_BUFFER = 1024;
	char buffer[MAX_BUFFER+1];
	size_t idx = 0;
	enum {
		NORMAL,
		BEGIN
	} state = NORMAL;

	while (*format && idx <= MAX_BUFFER) {
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
						if (idx >= MAX_BUFFER) break;
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
						if (idx >= MAX_BUFFER) break;
					}

					state = NORMAL;
				} else {
					buffer[idx++] = '?';
				}
				break;
		}

		format++;
	}

	va_end(args);

	buffer[idx] = 0;
	kprint(buffer);
}
