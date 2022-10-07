# Copyright (C) 2022 Mateus de Lima Oliveira

all:
	make -C input
	make -C database
	make -C codegen

install:
	make -C input install
	make -C database install
	make -C codegen install

clean:
	make -C input clean
	make -C database clean
	make -C codegen clean

.PHONY: all install clean
