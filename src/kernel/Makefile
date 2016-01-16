CC=../../bin/arm-non-eabi-gcc
CFLAGS=-mcpu=arm1176jzf-s -fpic -ffreestanding -std=gnu99 -Wall -Wextra

boot.o: boot.S
	$(CC) $(CFLAGS) -c boot.S -o boot.o

