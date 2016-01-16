CC=../../bin/arm-non-eabi-gcc
CXX=../../bin/arm-non-eabi-g++
OBJCOPY=../../bin/arm-non-eabi-objcopy
CFLAGS=-mcpu=arm1176jzf-s -fpic -ffreestanding -std=gnu99 -Wall -Wextra
CXXFLAGS=-mcpu=arm1176jzf-s -fpic -ffreestanding -std=c++11 -Wall -Wextra

OBJECTS=boot.o kernel.o csupport.o atag.o string.o mmio.o uart.o print.o

all: kernel.img

clean:
	-rm kernel.img kernel.elf $(OBJECTS)

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

kernel.elf: $(OBJECTS)
	$(CC) -T linker.ld -o kernel.elf -ffreestanding -O2 -nostdlib $(OBJECTS) -lgcc

kernel.img: kernel.elf
	$(OBJCOPY) kernel.elf -O binary kernel.img

