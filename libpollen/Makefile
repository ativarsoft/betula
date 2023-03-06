CFLAGS?=-Wall -Wextra -fPIC -I../include -g
LIBS=-llmdb -lsqlite3 -lvirt

PREFIX?=/usr

all: libpollen.a

libpollen.a: percent.o cookie.o query.o stream.o sanity.o system.o storage.o sql.o virt.o
	ar rcs $@ $^

install:
	install libpollen.a $(PREFIX)/lib/

clean:
	rm -f *.o *.a

.PHONY: clean install
