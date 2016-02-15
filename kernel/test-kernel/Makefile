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
CXXFLAGS += -I./include -I../shared-boot/include -I../../include -fno-exceptions -fno-rtti

OBJECTS=boot.o kernel.o led.o

ifeq ($(CI_BUILD),yes)
CISTUFF=
else
CISTUFF=
endif

all: obj test-kernel.bin $(CISTUFF)

clean:
	-rm -rf test-kernel.bin test-kernel.elf obj

obj/%.o: %.S
	$(CXX) $(CXXFLAGS) -c $< -o $@

obj/%.o: %.c *.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/%.o: %.cpp *.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

test-kernel.elf: $(addprefix obj/, $(OBJECTS)) ../lib/libshared-boot.a
	$(CXX) $(CXXFLAGS) -T linker.ld -o test-kernel.elf -ffreestanding -O2 -nostdlib $(addprefix obj/, $(OBJECTS)) -L../lib -lshared-boot -lgcc -lshared-boot

test-kernel.bin: test-kernel.elf
	$(OBJCOPY) test-kernel.elf -O binary test-kernel.bin

obj:
	test -d obj || mkdir obj
