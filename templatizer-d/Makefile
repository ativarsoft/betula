CFLAGS=-Wall -Wextra -fPIC -I../include
DMD=ldc2
ifeq ($(DMD),dmd)
DFLAGS=-fPIC -betterC
else
DFLAGS=-relocation-model=pic --betterC -nodefaultlib
endif
SOURCES=ctype.d  percent.d  platform.d  query.d  stream.d templatizer.d runtime.d storage.d

PREFIX?=/usr

all: libtemplatizer.a

%.o: %.d
	$(DMD) -c $(DFLAGS) -of="$@" $<

libtemplatizer.a: cookie.o safeint.o ctype.o percent.o platform.o query.o stream.o templatizer.o runtime.o storage.o
	ar rcs $@ $^

clean:
	rm -f *.o *.a

install: $(SOURCES)
	mkdir -p $(PREFIX)/lib/
	install libtemplatizer.a $(PREFIX)/lib/libtemplatizer-d.a
	mkdir -p $(PREFIX)/include/templatizer-d/
	install $(SOURCES) $(PREFIX)/include/templatizer-d/

.PHONY: all clean install
