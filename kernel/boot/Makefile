# Copyright (c) 2016, Kulshan Concepts
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#   1. Redistributions of source code must retain the above copyright notice,
#      this list of conditions and the following disclaimer.
#   2. Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#   3. Neither the name of the copyright holder nor the names of its
#      contributors may be used to endorse or promote products derived from
#      this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

include ../Makefile.defs

CFLAGS += -I./include -I../shared-boot/include -I../../include
CXXFLAGS += -I./include -I../shared-boot/include -I../../include

OBJECTS=boot.o sdp.o kernel.o

ifeq ($(CI_BUILD),yes)
CISTUFF=.CPPCHECK
else
CISTUFF=
endif

all: obj bootloader.bin $(CISTUFF)

clean:
	-rm -rf bootloader.bin bootloader.elf obj

obj/%.o: src/%.S
	$(CXX) $(CXXFLAGS) -c $< -o $@

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

obj/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

bootloader.elf: $(addprefix obj/, $(OBJECTS)) ../lib/libshared-boot.a
	$(CXX) -T linker.ld -o bootloader.elf -ffreestanding -O2 -nostdlib $(addprefix obj/, $(OBJECTS)) -lgcc -L../lib -lshared-boot

bootloader.bin: bootloader.elf
	$(OBJCOPY) bootloader.elf -O binary bootloader.bin

obj:
	test -d obj || mkdir obj

.CPPCHECK:
	cppcheck -I/opt/cross/lib/gcc/arm-none-eabi/5.3.0/include -Iinclude -I../shared-boot/include --std=c++11 --std=c99 --platform=unix64 --enable=all --force --xml --xml-version=2 src 2>cppcheck.xml
