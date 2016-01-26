include ../Makefile.defs

CFLAGS += -I./include -I../shared-boot/include
CXXFLAGS += -I./include -I../shared-boot/include

OBJECTS=boot.o sdp.o kernel.o

all: obj bootloader.bin

clean:
	-rm -rf bootloader.bin bootloader.elf obj

obj/%.o: src/%.S
	$(CXX) $(CXXFLAGS) -c $< -o $@

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

obj/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

bootloader.elf: $(addprefix obj/, $(OBJECTS)) ../lib/libshared-boot.a
	$(CXX) -T linker.ld -o bootloader.elf -ffreestanding -O2 -nostdlib $(addprefix obj/, $(OBJECTS)) -lgcc -L../lib -lshared-boot

bootloader.bin: bootloader.elf
	$(OBJCOPY) bootloader.elf -O binary bootloader.bin

obj:
	test -d obj || mkdir obj
