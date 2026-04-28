PREFIX?=/usr
PLUGIN_DIR?=$(PREFIX)/lib/pollen/plugins

all: target/release/libhelloworld_rs.so

target/release/libhelloworld_rs.so: src/lib.rs
	cargo build --release --offline

clean:
	cargo clean

install:
	mkdir -p $(PLUGIN_DIR)
	cp target/release/libhelloworld_rs.so $(PLUGIN_DIR)

.PHONY: helloworld_rs clean
