CFLAGS=-fPIC -Wall -Wpedantic -O0 -ggdb -I../../include
PLUGINS=sqlite.so sanity_checks.so
CFLAGS+=-L"../../libtemplatizer"

D=ldc2
ifeq ($(D),dmd)
DFLAGS=-fPIC -I../../templatizer-d/
else
DFLAGS=--betterC -relocation-model=pic -I../../templatizer-d/
endif

PREFIX?=/usr

%.o: %.d
	$(D) -c $(DFLAGS) -of="$@" $<

all: $(PLUGINS)

sqlite.so: sqlite.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^

sanity_checks.so: sanity_checks.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^

install:

clean:
	rm -f $(PLUGINS) *.o

.PHONY: install clean
