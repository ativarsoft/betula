CFLAGS=-fPIC -Wall -O0 -ggdb -Iinclude $(shell pkg-config --cflags apr-1)
EXEC=templatizer
VERSION=$(shell ./version.sh)
PREFIX?=/usr

all: $(EXEC) libtemplatizer templatizer-d plugins

deb: templatizer-$(VERSION).deb

dependencies:
	apt-get install $(shell cat dependencies.list)

plugins:
	make -C plugins

libtemplatizer:
	make -C libtemplatizer

templatizer-d:
	make -C templatizer-d

y.tab.c y.tab.y: calc.yacc
	yacc -d $<

lex.yy.c: calc.lex y.tab.h
	flex $<

lex.yy.o: lex.yy.c
y.tab.o: y.tab.c
templatizer.o: templatizer.c

templatizer: y.tab.o lex.yy.o templatizer.o interpreter.o opcode.o storage.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ -lexpat -ldl -lapr-1 -llmdb

test: templatizer plugins libtemplatizer templatizer-d
	$(MAKE) -C tests test

install: templatizer
	install templatizer $(PREFIX)/lib/cgi-bin/
	#install libtemplatizer/libtemplatizer.a $(PREFIX)/lib/
	cp include/templatizer.h $(PREFIX)/include
	cp -r include/templatizer/ $(PREFIX)/include/
	make -C plugins install
	make -C templatizer-d install

templatizer-$(VERSION).deb: templatizer include/templatizer.h conf/templatizer.conf.original
	./gen-control.sh
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
	dpkg-deb --build templatizer-$(VERSION)
	rm -r templatizer-$(VERSION)/

clean:
	rm -f $(EXEC) *.o lex.yy.c y.tab.c y.tab.h
	rm -fr templatizer-$(VERSION)
	rm -f templatizer-$(VERSION).deb
	make -C plugins clean
	make -C libtemplatizer clean
	make -C templatizer-d clean
	make -C tests clean

.PHONY: dependencies plugins libtemplatizer templatizer-d test install clean deb
