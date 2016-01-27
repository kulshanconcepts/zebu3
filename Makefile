export CI_BUILD

all: .UTIL .KERNEL

.KERNEL:
	$(MAKE) -C kernel

.UTIL:
	$(MAKE) -C util

clean:
	$(MAKE) -C kernel clean
	$(MAKE) -C util clean
