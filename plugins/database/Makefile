CFLAGS=-fPIC -Wall -Wpedantic -O0 -ggdb -I../../include
PLUGINS=kv.so
PREFIX?=/usr
INSTALL_DIR=$(PREFIX)/lib/pollen/plugins

all: $(PLUGINS)

kv.so: kv.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^

install: $(PLUGINS)
	mkdir -p $(INSTALL_DIR)
	cp -f $(PLUGINS) $(INSTALL_DIR)

clean:
	rm -f $(PLUGINS) *.o
