CC=../../bin/arm-non-eabi-gcc
CXX=../../bin/arm-non-eabi-g++
OBJCOPY=../../bin/arm-non-eabi-objcopy
CFLAGS=-mcpu=arm1176jzf-s -fpic -ffreestanding -std=gnu99 -Wall -Wextra
CXXFLAGS=-mcpu=arm1176jzf-s -fpic -ffreestanding -std=c++11 -Wall -Wextra

OBJECTS=boot.o kernel.o csupport.o atag.o string.o mmio.o uart.o print.o memory.o exception.o

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

