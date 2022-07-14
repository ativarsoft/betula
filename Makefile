CFLAGS=-fPIC -Wall -Wpedantic -O0 -ggdb -Iinclude
EXEC=templatizer

all: $(EXEC) plugins

plugins:
	make -C plugins

templatizer.o: templatizer.c

templatizer: templatizer.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ -lexpat -ldl

test: templatizer plugins
	$(MAKE) -C tests test

install: templatizer
	cp -f templatizer /usr/lib/cgi-bin/
	cp -f include/templatizer.h /usr/include
	make -C plugins install

clean:
	rm -f $(EXEC) *.o
	make -C plugins clean
	make -C tests clean

.PHONY: plugins test install clean
