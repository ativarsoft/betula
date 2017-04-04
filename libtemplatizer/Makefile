CFLAGS=-fPIC -Wall -Wpedantic -O0 -ggdb

all: libtemplatizer.so

libtemplatizer.so: http.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^ -lgnutls

clean:
	rm -f libtemplatizer.so *.o
