CC=../../bin/arm-non-eabi-gcc
OBJCOPY=../../bin/arm-non-eabi-objcopy
CFLAGS=-mcpu=arm1176jzf-s -fpic -ffreestanding -std=gnu99 -Wall -Wextra

OBJECTS=boot.o kernel.o

all: kernel.img

clean:
	-rm kernel.img kernel.elf $(OBJECTS)

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel.elf: $(OBJECTS)
	$(CC) -T linker.ld -o kernel.elf -ffreestanding -O2 -nostdlib $(OBJECTS)

kernel.img: kernel.elf
	$(OBJCOPY) kernel.elf -O binary kernel.img

