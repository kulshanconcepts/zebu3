CC=../../bin/arm-none-eabi-gcc
CXX=../../bin/arm-none-eabi-g++
OBJCOPY=../../bin/arm-none-eabi-objcopy
CFLAGS=-mcpu=arm1176jzf-s -fpic -ffreestanding -std=gnu99 -Wall -Wextra -I../shared-boot/
CXXFLAGS=-mcpu=arm1176jzf-s -fpic -ffreestanding -std=c++11 -Wall -Wextra -I../shared-boot/

OBJECTS=../shared-boot/boot.o ../shared-boot/csupport.o ../shared-boot/atag.o ../shared-boot/string.o ../shared-boot/mmio.o ../shared-boot/uart.o ../shared-boot/print.o memory.o exception.o kernel.o

all: kernel.bin

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

