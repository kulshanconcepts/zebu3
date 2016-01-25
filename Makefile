all: .SRC .UTIL

.SRC:
	$(MAKE) -C src

.UTIL:
	$(MAKE) -C util

clean:
	$(MAKE) -C src clean
	$(MAKE) -C util clean
