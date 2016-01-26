include ../Makefile.defs

CFLAGS += -I./include/
CXXFLAGS += -I./include/

OBJECTS=csupport.o atag.o string.o mmio.o uart.o

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
