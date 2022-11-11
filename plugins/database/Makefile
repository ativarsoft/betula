CFLAGS=-fPIC -Wall -Wpedantic -O0 -ggdb -I../../include
PLUGINS=kv.so

all: $(PLUGINS)

kv.so: kv.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^

install: $(PLUGINS)
	cp -f $(PLUGINS) $(PREFIX)/lib

clean:
	rm -f $(PLUGINS) *.o
