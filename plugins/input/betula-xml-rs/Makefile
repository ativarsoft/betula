all: build/libbetulaxml.a

build/libbetulaxml.a: src/lib.rs
ifeq ($(DEBUG),y)
	cargo build --out-dir="build/" -Z unstable-options
else
	cargo build --release --out-dir="build/" -Z unstable-options
endif

install:

clean:
	cargo clean

.PHONY: clean install

