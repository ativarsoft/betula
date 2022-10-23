CFLAGS=-Wall -Wextra -fPIC -I../include
DMD=dmd
DFLAGS=-fPIC

all: libtemplatizer.a

%.o: %.d
	$(DMD) -c $(DFLAGS) -of="$@" $<

libtemplatizer.a: percent.o cookie.o query.o stream.o
	ar rcs $@ $^

clean:
	rm -f *.o *.a
