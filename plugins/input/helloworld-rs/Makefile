PREFIX?=/usr
PLUGIN_DIR?=$(PREFIX)/lib/pollen/plugins

all: helloworld_rs

helloworld_rs:
	cargo build --release --offline

clean:
	cargo clean

install:
	mkdir -p $(PLUGIN_DIR)
	cp target/release/libhelloworld_rs.so $(PLUGIN_DIR)

.PHONY: helloworld_rs clean
