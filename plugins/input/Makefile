CFLAGS=-fPIC -Wall -Wpedantic -O0 -ggdb -I../../include
PLUGINS=in_helloworld.so in_percent.so

all: $(PLUGINS)

in_helloworld.so: helloworld.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^

in_percent.so: percent.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^

in_json.so: jsonc.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^ -llaxjson

install:

clean:
	rm -f $(PLUGINS) *.o

.PHONY: install clean
