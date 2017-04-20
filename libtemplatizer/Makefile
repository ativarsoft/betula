CFLAGS=-fPIC -Wall -Wpedantic -O0 -ggdb -I../include

all: libtemplatizer.so

libtemplatizer.so: http.o percent.o #json.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^ -lgnutls

install: libtemplatizer.so
	cp -f libtemplatizer.so /usr/lib

clean:
	rm -f libtemplatizer.so *.o
