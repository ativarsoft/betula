CFLAGS=-fPIC -Wall -Wpedantic -O0 -ggdb -I../../include
PLUGINS= headers.so copyright.so html.so noplugin.so

D=ldc2
ifeq ($(D),dmd)
DFLAGS=-fPIC -I../../templatizer-d/
else
DFLAGS=--betterC -relocation-model=pic -I../../templatizer-d/
endif

PREFIX?=/usr
INSTALL_DIR=$(PREFIX)/lib/pollen/plugins/

%.o: %.d
	$(D) -c $(DFLAGS) -of="$@" $<

all: $(PLUGINS)

html.so: html.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^

headers.so: headers.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^

copyright.so: copyright.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^

noplugin.so: noplugin.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^

install: $(PLUGINS)
	mkdir -p $(INSTALL_DIR)
	cp -f $(PLUGINS) $(INSTALL_DIR)

clean:
	rm -f $(PLUGINS) *.o

.PHONY: install clean
