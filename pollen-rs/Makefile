# Copyright (C) 2023 Mateus de Lima Oliveira

all:
	make -C pollen
	make -C pollen-sys

test:
	make -C pollen-sys test
	make -C pollen test

clean:
	make -C pollen-sys clean
	make -C pollen clean

.PHONY: pollen-sys pollen test clean
