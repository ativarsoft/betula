CFLAGS=-fPIC -Wall -Wpedantic -O0 -ggdb -I../../include

all: in_helloworld.so in_percent.so

in_helloworld.so: helloworld.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^

in_percent.so: percent.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^

in_json.so: jsonc.o
	$(CC) --shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@ $^ -llaxjson

install:
	#cp -f libtemplatizer.so /usr/lib

clean:
	rm -f libtemplatizer.so *.o
