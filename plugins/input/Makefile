CFLAGS=-fPIC -Wall -Wpedantic -O0 -ggdb -I../../include
PLUGINS=in_helloworld.so in_query.so
CFLAGS+=-L"../../libtemplatizer"

all: $(PLUGINS)

in_helloworld.so: helloworld.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^

in_query.so: in_query.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^ -ltemplatizer

in_json.so: jsonc.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^ -llaxjson

install:

clean:
	rm -f $(PLUGINS) *.o

.PHONY: install clean
