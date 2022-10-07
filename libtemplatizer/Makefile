CFLAGS=-Wall -Wextra -fPIC -I../include

all: libtemplatizer.a

libtemplatizer.a: percent.o cookie.o query.o
	ar rcs $@ $^

clean:
	rm -f *.o *.a
