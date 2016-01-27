export CI_BUILD

ifeq ($(CI_BUILD),yes)
CISTUFF=.DOXYGEN .SONAR
else
CISTUFF=
endif

all: lib out out/kernel.bin out/bootloader.bin $(CISTUFF)

clean:
	$(MAKE) -C shared-boot clean
	$(MAKE) -C boot clean
	$(MAKE) -C kernel clean
	-rm -rf out lib

lib/libshared-boot.a: shared-boot/libshared-boot.a
	cp shared-boot/libshared-boot.a $@

shared-boot/libshared-boot.a:
	$(MAKE) -C shared-boot

out/kernel.bin: kernel/kernel.bin
	cp kernel/kernel.bin $@

kernel/kernel.bin: lib/libshared-boot.a
	$(MAKE) -C kernel

out/bootloader.bin: boot/bootloader.bin
	cp $< $@

boot/bootloader.bin:
	$(MAKE) -C boot

lib:
	test -d lib || mkdir lib

out:
	test -d out || mkdir out

.DOXYGEN: out
	test -d out/doxygen || mkdir out/doxygen
	doxygen kernel.Doxyfile

.SONAR:
	/opt/sonar-scanner/bin/sonar-runner -X
