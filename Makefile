# Copyright (C) 2017-2023 Mateus de Lima Oliveira

APR_CFLAGS=$(shell pkg-config --cflags apr-1)

export CFLAGS=-fPIC -Wall -O0 -ggdb -I"$(shell pwd)/include" $(APR_CFLAGS)
LIBS?=-lexpat -ldl -lapr-1
EXEC=templatizer
export VERSION=$(shell ./version.sh)
export PREFIX?=/usr
export PLUGIN_DIR?=$(PREFIX)/lib/pollen/plugins/

ifeq ($(TERMUX),y)
CFLAGS+=-DTERMUX
LIBS=-lexpat -ldl -lapr-1 -lsqlite3
endif

ifeq ($(DEBUG),y)
CFLAGS+=-DDEBUG
endif

LIBYEAST_A=yeast/libyeast.a

HTML_PAGES=news.html
HTDOCS?=/var/www/html/

all: $(EXEC) libpollen templatizer-d templatizer-rs plugins $(HTML_PAGES)

deb: pollen-$(VERSION).deb

dependencies:
	apt-get install $(shell cat dependencies.list)

$(LIBYEAST_A): yeast/
	make -C yeast

plugins:
	make -C plugins

libpollen:
	make -C libpollen

templatizer-d:
	make -C templatizer-d

templatizer-rs:
ifneq ($(shell which cargo),)
	make -C templatizer-rs
endif

runtime/pollenrt0.o runtime/libpollen.a: runtime
	make -C runtime

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

templatizer: yeast/libyeast.a y.tab.o lex.yy.o pollen.o interpreter.o opcode.o jit.o linker.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

test: templatizer plugins libpollen templatizer-d runtime/pollenrt0.o runtime/libpollen.a
	$(MAKE) -C tests test

install: templatizer
	mkdir -p $(PREFIX)/lib/cgi-bin/
	mkdir -p $(PREFIX)/include
	mkdir -p $(PLUGIN_DIR)
	install templatizer $(PREFIX)/lib/cgi-bin/
	make -C libpollen install
	cp include/templatizer.h $(PREFIX)/include
	cp -r include/templatizer/ $(PREFIX)/include/
	cp -r include/pollen/ $(PREFIX)/include/
	make -C plugins install
	make -C templatizer-d install
	make -C apps install

debian/control: VERSION
	./gen-control.sh

pollen-$(VERSION).deb: debian/control templatizer include/templatizer.h conf/templatizer.conf.original
	mkdir -p pollen-$(VERSION)
	mkdir -p pollen-$(VERSION)/DEBIAN
	cp debian/control debian/postinst pollen-$(VERSION)/DEBIAN/
	chmod 0775 pollen-$(VERSION)/DEBIAN/*
	mkdir -p pollen-$(VERSION)/usr/lib/cgi-bin/
	cp templatizer pollen-$(VERSION)/usr/lib/cgi-bin/
	mkdir -p pollen-$(VERSION)/usr/include
	cp include/templatizer.h pollen-$(VERSION)/usr/include
	mkdir -p pollen-$(VERSION)/etc/apache2/conf-available/
	cp conf/templatizer.conf.original pollen-$(VERSION)/etc/apache2/conf-available/templatizer.conf
	cp conf/sysinfo.conf.original pollen-$(VERSION)/etc/apache2/conf-available/sysinfo.conf
	PREFIX="$(shell pwd)/pollen-$(VERSION)"/usr make install
	dpkg-deb --build pollen-$(VERSION)

clean:
	rm -f $(EXEC) *.o lex.yy.c y.tab.c y.tab.h
	rm -fr pollen-$(VERSION)/
	rm -f pollen-$(VERSION).deb
	rm -f $(HTML_PAGES)
	make -C yeast clean
	make -C plugins clean
	make -C libpollen clean
	make -C templatizer-d clean
	make -C runtime clean
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

debug:
	DEBUG="y" make

docker:
	docker build -t ativarsoft/pollen-$(VERSION) --no-cache .

news.html: news.xml log.xsl
	./news.sh

install-site: $(HTML_PAGES)
	cp *.html *.tmpl *.js $(HTDOCS)
	cp pollen.png  pollen-social-card.png  pollen-social-card.svg  pollen.svg $(HTDOCS)
	cp -r foundation/ $(HTDOCS)
	cp /usr/lib/pollen/plugins/* $(HTDOCS)

install-deb: pollen-$(VERSION).deb
	dpkg -i pollen-$(VERSION).deb

.PHONY: dependencies plugins libpollen templatizer-d templatizer-rs test install install-site clean deb termux $(LIBYEAST_A)

