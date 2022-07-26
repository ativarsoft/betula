CFLAGS=-fPIC -Wall -Wpedantic -O0 -ggdb -Iinclude
EXEC=templatizer

all: $(EXEC) plugins

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
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ -lexpat -ldl

test: templatizer plugins
	$(MAKE) -C tests test

install: templatizer
	cp -f templatizer /usr/lib/cgi-bin/
	cp -f include/templatizer.h /usr/include
	make -C plugins install

clean:
	rm -f $(EXEC) *.o lex.yy.c y.tab.c y.tab.h
	make -C plugins clean
	make -C tests clean

.PHONY: plugins test install clean
