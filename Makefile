CFLAGS=-fPIC -Wall -Wpedantic -O0 -ggdb
EXEC=templatizer test.so

all: $(EXEC)
	make -C libtemplatizer

templatizer.o: templatizer.c

templatizer: templatizer.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ -lexpat -ldl

test.so: test.o
	$(CC) $(LDFLAGS) --shared -o $@ $^

test: templatizer test.so
	$(MAKE) -C tests

install: templatizer
	cp -f templatizer /usr/lib/cgi-bin/
	cp -f -r include/templatizer /usr/include
	make -C libtemplatizer install

clean:
	rm -f $(EXEC) *.o
	rm -f -r db
	make -C libtemplatizer clean
