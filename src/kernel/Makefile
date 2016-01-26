CC=../../bin/arm-none-eabi-gcc
CXX=../../bin/arm-none-eabi-g++
OBJCOPY=../../bin/arm-none-eabi-objcopy
CFLAGS=-mcpu=cortex-a7 -mfpu=vfpv4 -fpic -ffreestanding -std=gnu99 -Wall -Wextra -I../shared-boot/
CXXFLAGS=-mcpu=cortex-a7 -mfpu=vfpv4 -fpic -ffreestanding -std=c++11 -Wall -Wextra -I../shared-boot/

OBJECTS=boot.o ../shared-boot/csupport.o ../shared-boot/atag.o ../shared-boot/string.o ../shared-boot/mmio.o ../shared-boot/uart.o print.o memory.o exception.o kernel.o logger.o sdp.o

ifeq ($(CI_BUILD),yes)
CISTUFF=.DOXYGEN .PUBLISH .CPPCHECK
else
CISTUFF=
endif

all: kernel.bin $(CISTUFF)

clean:
	-rm kernel.bin kernel.elf $(OBJECTS)

%.o: %.S
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

kernel.elf: $(OBJECTS)
	$(CXX) -T linker.ld -o kernel.elf -ffreestanding -O2 -nostdlib $(OBJECTS) -lgcc

kernel.bin: kernel.elf
	$(OBJCOPY) kernel.elf -O binary kernel.bin

.PUBLISH:
	-mkdir ../../out
	cp kernel.bin ../../out/kernel.img

.DOXYGEN:
	-mkdir ../../out
	-mkdir ../../out/doxygen
	-mkdir ../../out/doxygen/kernel
	doxygen kernel.Doxyfile

.CPPCHECK:
	-mkdir ../../out
	-mkdir ../../out/cppcheck
	cppcheck -I../shared-boot --std=c++11 --std=c99 --platform=unix64 --xml --xml-version=2 2>../../out/cppcheck/kernel.xml .
