include ../Makefile.defs

CFLAGS += -I./include/
CXXFLAGS += -I./include/

OBJECTS=csupport.o atag.o string.o mmio.o uart.o

ifeq ($(CI_BUILD),yes)
CISTUFF=.CPPCHECK
else
CISTUFF=
endif

all: obj libshared-boot.a

clean:
	-rm -rf shared-boot.a obj

obj/%.o: src/%.S
	$(CXX) $(CXXFLAGS) -c $< -o $@

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

obj/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

libshared-boot.a: $(addprefix obj/, $(OBJECTS))
	$(AR) $(ARFLAGS) $@ $^

obj:
	test -d obj || mkdir obj

.CPPCHECK:
	cppcheck -I/opt/cross/lib/gcc/arm-none-eabi/5.3.0/include -Iinclude --std=c++11 --std=c99 --platform=unix64 --enable=all --force --xml --xml-version=2 src 2>cppcheck.xml
