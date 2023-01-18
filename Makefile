# Copyright (C) 2017-2023 Mateus de Lima Oliveira

CFLAGS=-fPIC -Wall -O0 -ggdb -Iinclude $(shell pkg-config --cflags apr-1)
LIBS?=-lexpat -ldl -lapr-1 -llmdb -lsqlite3 -lvirt
EXEC=templatizer
VERSION=$(shell ./version.sh)
PREFIX?=/usr

ifeq ($(TERMUX),y)
CFLAGS+=-DTERMUX
LIBS=-lexpat -ldl -lapr-1 -lsqlite3
endif

all: $(EXEC) libtemplatizer templatizer-d templatizer-rs plugins

deb: templatizer-$(VERSION).deb

dependencies:
	apt-get install $(shell cat dependencies.list)

plugins:
	make -C plugins

libtemplatizer:
	make -C libtemplatizer

templatizer-d:
	make -C templatizer-d

templatizer-rs:
ifneq ($(shell which cargo),)
	make -C templatizer-rs
endif

y.tab.c y.tab.y: calc.yacc
	yacc -d $<

lex.yy.c: calc.lex y.tab.h
	flex $<

lex.yy.o: lex.yy.c
y.tab.o: y.tab.c
sql.o: sql.c sql.h
interpreter.o: interpreter.c interpreter.h
opcode.o: opcode.c opcode.h
storage.o: storage.c storage.h
templatizer.o: templatizer.c

templatizer: y.tab.o lex.yy.o templatizer.o interpreter.o opcode.o storage.o sql.o virt.o regex.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

test: templatizer plugins libtemplatizer templatizer-d
	$(MAKE) -C tests test

install: templatizer
	mkdir -p $(PREFIX)/lib/cgi-bin/
	mkdir -p $(PREFIX)/include
	install templatizer $(PREFIX)/lib/cgi-bin/
	#install libtemplatizer/libtemplatizer.a $(PREFIX)/lib/
	cp include/templatizer.h $(PREFIX)/include
	cp -r include/templatizer/ $(PREFIX)/include/
	make -C plugins install
	make -C templatizer-d install
	make -C apps install

debian/control: VERSION
	./gen-control.sh

templatizer-$(VERSION).deb: debian/control templatizer include/templatizer.h conf/templatizer.conf.original
	mkdir -p templatizer-$(VERSION)
	mkdir -p templatizer-$(VERSION)/DEBIAN
	cp debian/control debian/postinst templatizer-$(VERSION)/DEBIAN/
	chmod 0775 templatizer-$(VERSION)/DEBIAN/*
	mkdir -p templatizer-$(VERSION)/usr/lib/cgi-bin/
	cp templatizer templatizer-$(VERSION)/usr/lib/cgi-bin/
	mkdir -p templatizer-$(VERSION)/usr/include
	cp include/templatizer.h templatizer-$(VERSION)/usr/include
	mkdir -p templatizer-$(VERSION)/etc/apache2/conf-available/
	cp conf/templatizer.conf.original templatizer-$(VERSION)/etc/apache2/conf-available/templatizer.conf
	PREFIX="$(shell pwd)/templatizer-$(VERSION)"/usr make install
	dpkg-deb --build templatizer-$(VERSION)

clean:
	rm -f $(EXEC) *.o lex.yy.c y.tab.c y.tab.h
	rm -fr templatizer-$(VERSION)/
	rm -f templatizer-$(VERSION).deb
	make -C plugins clean
	make -C libtemplatizer clean
	make -C templatizer-d clean
ifneq ($(shell which cargo),)
	make -C templatizer-rs clean
endif
	make -C apps clean
	make -C tests clean

# MISRA-C standard compliance check for
# failproof software
misra: templatizer.c
	cppcheck --addon=misra.py $^

termux:
	TERMUX="y" make

.PHONY: dependencies plugins libtemplatizer templatizer-d templatizer-rs test install clean deb termux
