include ../Makefile.defs

CFLAGS += -I./include -I../shared-boot/include
CXXFLAGS += -I./include -I../shared-boot/include

OBJECTS=boot.o print.o memory.o exception.o kernel.o logger.o sdp.o

ifeq ($(CI_BUILD),yes)
CISTUFF=.CPPCHECK
else
CISTUFF=
endif

all: obj kernel.bin $(CISTUFF)

clean:
	-rm -rf kernel.bin kernel.elf obj

obj/%.o: src/%.S
	$(CXX) $(CXXFLAGS) -c $< -o $@

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

obj/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

kernel.elf: $(addprefix obj/, $(OBJECTS)) ../lib/libshared-boot.a
	$(CXX) -T linker.ld -o kernel.elf -ffreestanding -O2 -nostdlib $(addprefix obj/, $(OBJECTS)) -lgcc -L../lib -lshared-boot

kernel.bin: kernel.elf
	$(OBJCOPY) kernel.elf -O binary kernel.bin

.CPPCHECK:
	cppcheck -I/opt/cross/lib/gcc/arm-none-eabi/5.3.0/include -Iinclude -I../shared-boot/include --std=c++11 --std=c99 --platform=unix64 --enable=all --force --xml --xml-version=2 src 2>cppcheck.xml

obj:
	test -d obj || mkdir obj
