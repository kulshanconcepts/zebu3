ifeq ($(CI_BUILD),yes)
CISTUFF=.SONAR
else
CISTUFF=
endif

all: .BOOT .KERNEL .SDPCLIENT $(CISTUFF)

.BOOT:
	$(MAKE) -C boot

.KERNEL:
	$(MAKE) -C kernel

.SDPCLIENT:
	$(MAKE) -C sdpclient

clean:
	$(MAKE) -C boot clean
	$(MAKE) -C kernel clean
	$(MAKE) -C sdpclient clean

.SONAR:
	/opt/sonar-scanner/bin/sonar-runner -X
