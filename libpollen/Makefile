CFLAGS=-Wall -Wextra -fPIC -I../include -g

PREFIX?=/usr

all: libpollen.a

libpollen.a: percent.o cookie.o query.o stream.o sanity.o system.o
	ar rcs $@ $^

install:
	install libpollen.a $(PREFIX)/lib/

clean:
	rm -f *.o *.a

.PHONY: clean install
