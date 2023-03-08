# Copyright (C) 2022 Mateus de Lima Oliveira

PREFIX?=/usr

export PREFIX

all:
	make -C input
	make -C output
	make -C net
	make -C database
	make -C codegen
	make -C linker
	make -C app
	make -C tests

install:
	mkdir -p $(PREFIX)/lib/templatizer/plugins/
	make -C input install
	make -C output install
	make -C net install
	make -C database install
	make -C codegen install
	make -C linker install
	make -C app install
	make -C tests install

clean:
	make -C input clean
	make -C output clean
	make -C net clean
	make -C database clean
	make -C codegen clean
	make -C linker clean
	make -C app clean
	make -C tests clean

.PHONY: all install clean
