CFLAGS=-fPIC -Wall -Wpedantic -O0 -ggdb -I../../include
PLUGINS=echo.so
CFLAGS+=-L"../../libtemplatizer"

PREFIX?=/usr
INSTALL_DIR=$(PREFIX)/lib/pollen/plugins

D=ldc2

ifeq ($(DMD),dmd)
DFLAGS=-fPIC -betterC -I../../templatizer-d/
else
DFLAGS=-relocation-model=pic --betterC -nodefaultlib -I../../templatizer-d/
endif

PREFIX?=/usr

BUILD_PROFILE?=release

%.o: %.d
	$(D) -c $(DFLAGS) -of="$@" $<

all: $(PLUGINS)

echo.so: echo.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^

install:
	mkdir -p $(INSTALL_DIR)
	cp $(PLUGINS) $(INSTALL_DIR)

clean:
	rm -f $(PLUGINS) *.o

.PHONY: install clean
