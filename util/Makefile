export CI_BUILD

all: .CROSS .SDPCLIENT

.SDPCLIENT:
	$(MAKE) -C sdpclient

.CROSS:
ifneq ($(CI_BUILD),yes)
	$(MAKE) -C cross
endif

clean:
	$(MAKE) -C cross clean
	$(MAKE) -C sdpclient clean
