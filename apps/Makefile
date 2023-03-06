PREFIX?=/usr

all: quick-cloud sysinfo

quick-cloud:
	make -C quick-cloud

sysinfo:
	make -C sysinfo

install:
	make -C quick-cloud install
	make -C sysinfo install

clean:
	make -C quick-cloud clean
	make -C sysinfo clean

.PHONY: install clean quick-cloud sysinfo
