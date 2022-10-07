CFLAGS=-Wall -Wextra -fPIC -I../../include/
PLUGINS=cd_i386.so

all: $(PLUGINS)

cd_i386.so: cd_i386.o
	$(CC) $(CFLAGS) --shared -o $@ $^

install:

clean:
	rm -f $(PLUGINS) *.o

.PHONY: install clean
