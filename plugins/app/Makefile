CFLAGS=-fPIC -Wall -Wpedantic -O0 -ggdb -I../../include
ifneq ($(shell which ldc2),)
DPLUGINS=dating.so nslookup.so
endif
PLUGINS=$(DPLUGINS)
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

dating.so: dating.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^

nslookup.so: nslookup.o inetaddr.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^

install:

clean:
	rm -f $(PLUGINS) *.o

.PHONY: install clean
