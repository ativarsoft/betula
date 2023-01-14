CFLAGS=-fPIC -Wall -Wpedantic -O0 -ggdb -I../../include
PLUGINS=in_helloworld.so in_query.so helloworld_d.so true.so false.so
CFLAGS+=-L"../../libtemplatizer"

D=ldc2
ifeq ($(D),dmd)
DFLAGS=-fPIC -I../../templatizer-d/
else
DFLAGS=-relocation-model=pic -I../../templatizer-d/
endif

PREFIX?=/usr

BUILD_PROFILE?=release

%.o: %.d
	$(D) -c $(DFLAGS) -of="$@" $<

all: $(PLUGINS) helloworld-rs

in_helloworld.so: helloworld.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^

true.so: true.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^

false.so: false.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^

helloworld_d.so: helloworld_d.o ../../templatizer-d/libtemplatizer.a
	$(D) -shared $(DFLAGS) -of=$@ $^

helloworld-rs:
ifneq ($(shell which cargo),)
	make -C helloworld-rs
endif

in_query.so: in_query.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^ -ltemplatizer

in_json.so: jsonc.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^ -llaxjson

install:

clean:
	rm -f $(PLUGINS) *.o
ifneq ($(shell which cargo),)
	make -C helloworld-rs clean
endif

.PHONY: helloworld-rs install clean
