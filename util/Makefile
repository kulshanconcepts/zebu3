all: .CROSS .SDPCLIENT

.SDPCLIENT:
	$(MAKE) -C sdpclient

.CROSS:
	$(MAKE) -C cross

clean:
	$(MAKE) -C cross clean
	$(MAKE) -C sdpclient clean
