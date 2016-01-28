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

all: ../../bin/arm-none-eabi-gcc

clean:
	-rm -rf build-gcc build-binutils build-gdb ../../bin/arm-none-eabi-* ../../arm-none-eabi

distclean: clean
	-rm -rf gcc binutils gdb

../../bin/arm-none-eabi-gcc: ../../bin/arm-none-eabi-as gcc/.extracted
	TARGET="arm-none-eabi" ./build-gcc.sh

../../bin/arm-none-eabi-as: binutils/.extracted
	TARGET="arm-none-eabi" ./build-binutils.sh

../../bin/arm-none-eabi-gdb: gdb/.extracted
	TARGET="arm-none-eabi" ./build-gdb.sh

gcc/.extracted: gcc-5.3.0.tar.gz
	test -d gcc || mkdir gcc
	tar -xzf gcc-5.3.0.tar.gz -C gcc --strip-components=1
	touch $@

binutils/.extracted: binutils-2.25.1.tar.gz
	test -d binutils || mkdir binutils
	tar -xzf binutils-2.25.1.tar.gz -C binutils --strip-components=1
	touch $@

gdb/.extracted: gdb-7.10.1.tar.gz
	test -d gdb || mkdir gdb
	tar -xzf gdb-7.10.1.tar.gz -C gdb --strip-components=1
	touch $@

gcc-5.3.0.tar.gz:
	wget -nv http://mirrors-usa.go-parts.com/gcc/releases/gcc-5.3.0/gcc-5.3.0.tar.gz

binutils-2.25.1.tar.gz:
	wget -nv http://ftp.gnu.org/gnu/binutils/binutils-2.25.1.tar.gz

gdb-7.10.1.tar.gz:
	wget -nv http://ftp.gnu.org/gnu/gdb/gdb-7.10.1.tar.gz
