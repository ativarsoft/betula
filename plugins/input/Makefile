# Copyright (C) 2023 Mateus de Lima Oliveira

CFLAGS=-fPIC -Wall -Wpedantic -O0 -ggdb -I../../include
PLUGINS=in_helloworld.so in_query.so helloworld_d.so true.so false.so \
  null.so
CFLAGS+=-L"../../libpollen"

# libpollen is linked by default. A typical program in Pollen language
# expects and uses functions provided by the libpollen runtime.
#
# If the program is being interpreted, the symbols are loaded dynamically
# on runtime by the dynamic linker. The dynamic linker in linux is libdl.
# The libdl library provides the functions dlopen and dlsym that the
# interpreter uses to retrieve the function and variable memory addresses.
# Those addresses are known as symbols.
#
# If the program is being compiled, the compiler creates object code with
# references to the function and variables on selected input library, which
# are resolved by the linker on link time.
#
# -- Mateus de Lima Oliveira
LIBS=-lpollen

D=ldc2
ifeq ($(D),dmd)
DFLAGS=-fPIC -I../../templatizer-d/
else
DFLAGS=-relocation-model=pic -I../../templatizer-d/
endif

PREFIX?=/usr
INSTALL_DIR=$(PREFIX)/lib/pollen/plugins/

BUILD_PROFILE?=release

%.o: %.d
	$(D) -c $(DFLAGS) -of="$@" $<

all: $(PLUGINS) helloworld-rs

null.so: null.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

sysinfo.so: sysinfo.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

in_helloworld.so: helloworld.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

true.so: true.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

false.so: false.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

helloworld_d.so: helloworld_d.o ../../templatizer-d/libtemplatizer.a
	$(D) -shared $(DFLAGS) -of=$@ $^

helloworld-rs:
ifneq ($(shell which cargo),)
	make -C helloworld-rs
endif

in_query.so: in_query.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

in_json.so: jsonc.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^ -llaxjson

install:
	mkdir -p $(INSTALL_DIR)
	cp $(PLUGINS) $(INSTALL_DIR)

clean:
	rm -f $(PLUGINS) *.o
ifneq ($(shell which cargo),)
	make -C helloworld-rs clean
endif

.PHONY: helloworld-rs install clean
