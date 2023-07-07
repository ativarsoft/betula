# Copyright (C) 2017-2023 Mateus de Lima Oliveira

APR_CFLAGS=$(shell pkg-config --cflags apr-1)

export CFLAGS=-fPIC -Wall -O0 -ggdb -I"$(shell pwd)/include" $(APR_CFLAGS)
LIBS?=-lexpat -ldl -lapr-1
EXEC=templatizer
export VERSION=$(shell ./version.sh)
export PREFIX?=/usr
export PLUGIN_DIR?=$(PREFIX)/lib/pollen/plugins/

ifneq ($(TERMUX_VERSION),)
CFLAGS+=-DTERMUX
LIBS=-lexpat -ldl -lapr-1 -lsqlite3
endif

ifeq ($(DEBUG),y)
CFLAGS+=-DDEBUG
endif

include dlang.mk

LIBYEAST_A=yeast/libyeast.a

# Avoid HTML content that may seem uninteligible to non-developers as
# search engines categorise them as placeholder text in drafts.
HTML_PAGES= #notes.html
HTDOCS?=/var/www/betula/

all: $(EXEC) libpollen templatizer-d pollen-rs plugins $(HTML_PAGES)

deb: pollen-$(VERSION).deb

dependencies:
ifneq ($(shell which apt-get),)
ifneq ($(TERMUX_VERSION),)
	apt-get install $(shell cat dependencies.termux)
else
	apt-get install $(shell cat dependencies.list)
endif
endif
ifneq ($(shell which apk),)
	apk add $(shell cat dependencies-alpine.list)
endif

$(LIBYEAST_A): yeast/
	make -C yeast

plugins:
	make -C plugins

libpollen:
	make -C libpollen

templatizer-d:
	make -C templatizer-d

pollen-rs:
ifneq ($(shell which cargo),)
	make -C pollen-rs
else
	$(warning not building pollen-rs/ as cargo was not found)
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

templatizer: yeast/libyeast.a y.tab.o lex.yy.o pollen.o interpreter.o opcode.o jit.o linker.o linker_d.o interpreter_d.o list_d.o xml_d.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

test: templatizer plugins libpollen templatizer-d runtime/pollenrt0.o runtime/libpollen.a
ifneq ($(shell which cargo),)
	$(MAKE) -C pollen-rs --jobs=1
endif
	$(MAKE) -C tests test --jobs=1

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

debian/control: VERSION gen-control.sh
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
	mkdir -p pollen-$(VERSION)/etc/apache2/sites-available/
	cp conf/betula.conf pollen-$(VERSION)/etc/apache2/sites-available/
	mkdir -p pollen-$(VERSION)/$(HTDOCS)
	PREFIX="$(shell pwd)/pollen-$(VERSION)"/usr make install
	dpkg-deb --build pollen-$(VERSION)

clean: alire-clean
	rm -f $(EXEC) *.o lex.yy.c y.tab.c y.tab.h
	rm -fr pollen-$(VERSION)/
	make -C yeast clean
	make -C plugins clean
	make -C libpollen clean
	make -C templatizer-d clean
	make -C runtime clean
ifneq ($(shell which cargo),)
	make -C pollen-rs clean
endif
	make -C apps clean
	make -C tests clean

dist-clean: clean
	rm -f pollen-$(VERSION).deb

site-clean:
	# HTML pages are required by GitHub pages
	rm -f $(HTML_PAGES)

# MISRA-C standard compliance check for
# failproof software
misra: pollen.c
	cppcheck --addon=misra.py $^

clang-analyser: pollen.c
	scan-build make

debug:
	DEBUG="y" make

docker:
	docker build -t ativarsoft/betula:$(VERSION) --no-cache .

docker-website:
	docker build -t ativarsoft/betula-website:$(VERSION) --no-cache --file Dockerfile.website .

docker-push:
	docker push ativarsoft/betula:$(VERSION)

docker-website-push:
	docker push ativarsoft/betula-website:$(VERSION)

docker-run:
	docker run -p12000:80 ativarsoft/betula:$(VERSION)

docker-website-run:
	docker run -p12001:12001 ativarsoft/betula-website:$(VERSION)

docker-stop-all:
	docker kill $(docker ps -q)

docker-rm-all:
	docker ps -aq | xargs docker stop | xargs docker rm

notes.html: notes.xml log.xsl
	./notes.sh

install-site: $(HTML_PAGES)
	mkdir -p $(HTDOCS)
	cp *.html *.tmpl *.js $(HTDOCS)
	cp pollen.png  pollen-social-card.png  pollen-social-card.svg  pollen.svg $(HTDOCS)
	cp -r foundation/ $(HTDOCS)
	cp $(PREFIX)/lib/pollen/plugins/* $(HTDOCS)
	chmod g+w $(HTDOCS)
	chown www-data:root $(HTDOCS)

install-deb: pollen-$(VERSION).deb
	dpkg -i pollen-$(VERSION).deb

alire: install-alire.sh
	#./install-alire.sh install

alire-clean: install-alire.sh
	#./install-alire.sh remove

strip: $(EXEC)
	strip $(EXEC)

.PHONY: dependencies plugins libpollen templatizer-d pollen-rs \
  test deb $(LIBYEAST_A) \
  install install-site \
  clean dist-clean site-clean alire-clean \
  docker docker-push docker-run \
  strip

