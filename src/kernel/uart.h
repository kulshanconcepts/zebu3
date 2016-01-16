// Copyright (c) 2016 Kulshan Concepts. All rights reserved.

#include <stddef.h>
#include <stdint.h>

class Uart {
private:
	static Uart* instance;
	
public:
	static const Uart* getInstance();
	
	Uart();
	void putc(uint8_t byte) const;
	uint8_t getc() const;
	void write(const char* buffer, size_t size) const;
	void puts(const char* str) const;
};
