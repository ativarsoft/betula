CFLAGS=-Wall -Wextra -fPIC -I../include
DMD=dmd
DFLAGS=-fPIC -betterC
SOURCES=ctype.d  percent.d  platform.d  query.d  stream.d

PREFIX?=/usr

all: libtemplatizer.a

%.o: %.d
	$(DMD) -c $(DFLAGS) -of="$@" $<

libtemplatizer.a: templatizer.o ctype.o percent.o platform.o query.o stream.o
	ar rcs $@ $^

clean:
	rm -f *.o *.a

install: $(SOURCES)
	mkdir -p $(PREFIX)/include/templatizer-d/
	install $(SOURCES) $(PREFIX)/include/templatizer-d/

.PHONY: all clean install
