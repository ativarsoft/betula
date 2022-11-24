CFLAGS=-fPIC -Wall -Wpedantic -O0 -ggdb -I../../include
PLUGINS=in_helloworld.so in_query.so helloworld_d.so
CFLAGS+=-L"../../libtemplatizer"

D=ldc2
ifeq ($(D),dmd)
DFLAGS=-fPIC -I../../templatizer-d/
else
DFLAGS=-relocation-model=pic -I../../templatizer-d/
endif

PREFIX?=/usr

%.o: %.d
	$(D) -c $(DFLAGS) -of="$@" $<

all: $(PLUGINS)

in_helloworld.so: helloworld.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^

helloworld_d.so: helloworld_d.o ../../templatizer-d/libtemplatizer.a
	$(D) -shared $(DFLAGS) -of=$@ $^

in_query.so: in_query.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^ -ltemplatizer

in_json.so: jsonc.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^ -llaxjson

install:

clean:
	rm -f $(PLUGINS) *.o

.PHONY: install clean
