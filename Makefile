CFLAGS=-fPIC -Wall -O0 -ggdb -Iinclude
EXEC=templatizer
VERSION=0.8

all: $(EXEC) plugins templatizer-$(VERSION).deb

plugins:
	make -C plugins

y.tab.c y.tab.y: calc.yacc
	yacc -d $<

lex.yy.c: calc.lex y.tab.h
	flex $<

lex.yy.o: lex.yy.c
y.tab.o: y.tab.c
templatizer.o: templatizer.c

templatizer: y.tab.o lex.yy.o templatizer.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ -lexpat -ldl -lapr-1

test: templatizer plugins
	$(MAKE) -C tests test

install: templatizer
	cp -f templatizer /usr/lib/cgi-bin/
	cp -f include/templatizer.h /usr/include
	make -C plugins install

templatizer-$(VERSION).deb: templatizer include/templatizer.h conf/templatizer.conf.original
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

clean:
	rm -f $(EXEC) *.o lex.yy.c y.tab.c y.tab.h
	rm -fr templatizer-$(VERSION)
	rm -f templatizer-$(VERSION).deb
	make -C plugins clean
	make -C tests clean

.PHONY: plugins test install clean
