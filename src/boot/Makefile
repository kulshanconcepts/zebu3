CC=../../bin/arm-none-eabi-gcc
CXX=../../bin/arm-none-eabi-g++
OBJCOPY=../../bin/arm-none-eabi-objcopy
CFLAGS=-mcpu=arm1176jzf-s -fpic -ffreestanding -std=gnu99 -Wall -Wextra -I../shared-boot/
CXXFLAGS=-mcpu=arm1176jzf-s -fpic -ffreestanding -std=c++11 -Wall -Wextra -I../shared-boot/

OBJECTS=boot.o ../shared-boot/csupport.o ../shared-boot/atag.o ../shared-boot/string.o ../shared-boot/mmio.o ../shared-boot/uart.o ../shared-boot/print.o sdp.o kernel.o

all: bootloader.bin

clean:
	-rm bootloader.bin bootloader.elf $(OBJECTS)

%.o: %.S
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

bootloader.elf: $(OBJECTS)
	$(CXX) -T linker.ld -o bootloader.elf -ffreestanding -O2 -nostdlib $(OBJECTS) -lgcc

bootloader.bin: bootloader.elf
	$(OBJCOPY) bootloader.elf -O binary bootloader.bin
