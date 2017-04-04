CFLAGS=-fPIC -Wall -Wpedantic -O0 -ggdb
EXEC=templatizer libtemplatizer.so test.so

all: $(EXEC)
	make -C libtemplatizer

templatizer.o: templatizer.c templatizer.h

templatizer: templatizer.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ -lexpat -ldl

test.so: test.o
	$(CC) $(LDFLAGS) --shared -o $@ $^

test: templatizer test.so
	$(MAKE) -C tests

install: templatizer
	cp templatizer /usr/lib/cgi-bin/

clean:
	rm -f $(EXEC) *.o
	rm -f -r db
	make -C libtemplatizer clean
