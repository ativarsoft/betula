CFLAGS=-fPIC -Wall -Wpedantic -O0 -ggdb -I../../include
PLUGINS= headers.so copyright.so html.so

all: $(PLUGINS)

html.so: html.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^

headers.so: headers.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^

copyright.so: copyright.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^

install: $(PLUGINS)
	#cp -f $(PLUGINS) /usr/lib

clean:
	rm -f $(PLUGINS) *.o
