all: ../../bin/arm-non-eabi-gcc ../../bin/arm-non-eabi-gdb

clean:
	-rm -rf build-gcc build-binutils build-gdb ../../bin/arm-non-eabi-* ../../arm-non-eabi

distclean: clean
	-rm -rf gcc binutils gdb

../../bin/arm-non-eabi-gcc: ../../bin/arm-non-eabi-as gcc
	TARGET="arm-non-eabi" ./build-gcc.sh

../../bin/arm-non-eabi-as: binutils
	TARGET="arm-non-eabi" ./build-binutils.sh

../../bin/arm-non-eabi-gdb: gdb
	TARGET="arm-non-eabi" ./build-gdb.sh

gcc: gcc-5.3.0.tar.gz
	-mkdir gcc
	tar -xzf gcc-5.3.0.tar.gz -C gcc --strip-components=1

binutils: binutils-2.25.1.tar.gz
	-mkdir binutils
	tar -xzf binutils-2.25.1.tar.gz -C binutils --strip-components=1

gdb: gdb-7.10.1.tar.gz
	-mkdir gdb
	tar -xzf gdb-7.10.1.tar.gz -C gdb --strip-components=1

gcc-5.3.0.tar.gz:
	wget -nv http://mirrors-usa.go-parts.com/gcc/releases/gcc-5.3.0/gcc-5.3.0.tar.gz

binutils-2.25.1.tar.gz:
	wget -nv http://ftp.gnu.org/gnu/binutils/binutils-2.25.1.tar.gz

gdb-7.10.1.tar.gz:
	wget -nv http://ftp.gnu.org/gnu/gdb/gdb-7.10.1.tar.gz

