# Copyright (C) 2022 Mateus de Lima Oliveira

all:
	make -C input
	make -C database
	make -C codegen
	make -C linker
	make -C app

install:
	make -C input install
	make -C database install
	make -C codegen install
	make -C linker install
	make -C app install

clean:
	make -C input clean
	make -C database clean
	make -C codegen clean
	make -C linker clean
	make -C app clean

.PHONY: all install clean
