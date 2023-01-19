CFLAGS=-fPIC -Wall -Wpedantic -O0 -ggdb -I../../include
PLUGINS=echo.so
CFLAGS+=-L"../../libtemplatizer"

PREFIX?=/usr

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

all: $(PLUGINS)

echo.so: echo.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^

install:

clean:
	rm -f $(PLUGINS) *.o

.PHONY: install clean
