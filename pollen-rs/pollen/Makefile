# Copyright (C) 2023 Mateus de Lima Oliveira

all: templatizer

templatizer:
	cargo build --release --offline

test:
	RUST_BACKTRACE="1" cargo test

clean:
	cargo clean

.PHONY: templatizer test clean
