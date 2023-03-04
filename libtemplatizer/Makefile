CFLAGS=-Wall -Wextra -fPIC -I../include -g

PREFIX?=/usr

all: libtemplatizer.a

libtemplatizer.a: percent.o cookie.o query.o stream.o
	ar rcs $@ $^

install:
	install libtemplatizer.a $(PREFIX)/lib/

clean:
	rm -f *.o *.a

.PHONY: clean install
