all: pollenrt0.o libpollen.a

pollenrt0.o: pollenrt0.c

stdlib.o: stdlib.c

libpollen.a: stdlib.o
	ar rcs $@ $^

install:
	install pollenrt0.o $(PREFIX)/lib/
	install libpollen.a $(PREFIX)/lib/

clean:
	rm -f *.o *.a *.so

.PHONY: clean install
