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

export CI_BUILD

ifeq ($(CI_BUILD),yes)
CISTUFF=.DOXYGEN .SONAR
else
CISTUFF=
endif

export CPPCHECKFLAGS= --std=c++11 --std=c99 --platform=unix64 --enable=all --force --xml --xml-version=2 \
 	-D__INTPTR_TYPE__ -D__UINT16_TYPE__ -D__UINT32_TYPE__ -D__UINT64_TYPE__ -D_TYPE_ptrdiff_t \
	-D_TYPE_size_t -D_TYPE_wchar_t -D__INT8_TYPE__ -D__UINT8_TYPE__ -D__UINTPTR_TYPE__

all: lib out out/kernel.bin out/bootloader.bin $(CISTUFF)

clean:
	$(MAKE) -C shared-boot clean
	$(MAKE) -C boot clean
	$(MAKE) -C kernel clean
	-rm -rf out lib

lib/libshared-boot.a: .SHARED_BOOT
	cp shared-boot/libshared-boot.a $@

.SHARED_BOOT:
	$(MAKE) -C shared-boot

out/kernel.bin: .KERNEL
	cp kernel/kernel.bin out/kernel.bin

.KERNEL: .SHARED_BOOT
	$(MAKE) -C kernel

out/bootloader.bin: .BOOTLOADER
	cp boot/bootloader.bin out/bootloader.bin

.BOOTLOADER: .SHARED_BOOT
	$(MAKE) -C boot

lib:
	test -d lib || mkdir lib

out:
	test -d out || mkdir out

.DOXYGEN: out
	test -d out/doxygen || mkdir out/doxygen
	doxygen kernel.Doxyfile

.SONAR:
	/opt/sonar-scanner/bin/sonar-runner -X
