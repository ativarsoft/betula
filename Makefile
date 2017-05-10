CFLAGS=-fPIC -Wall -Wpedantic -O0 -ggdb -Iinclude
EXEC=templatizer kv rest

all: $(EXEC)

libtemplatizer/libtemplatizer.so: libtemplatizer
	make -C libtemplatizer

templatizer.o: templatizer.c

templatizer: templatizer.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ -lexpat -ldl

kv: kv.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ -llmdb

rest: rest.o libtemplatizer/libtemplatizer.so
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ -ltemplatizer -lcurl

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
