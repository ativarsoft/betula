CFLAGS=-Wall -Wextra -fPIC -I../../include
PLUGINS=ld-ada.so ld-exec.so

PREFIX?=/usr
INSTALL_DIR=$(PREFIX)/lib/pollen/plugins

all: $(PLUGINS)

ld-ada.so: ld-ada.o
	$(CC) $(CFLAGS) -shared -o$@ $^ -ldl

ld-exec.so: ld-exec.o
	$(CC) $(CFLAGS) -shared -o$@ $^

install: $(PLUGINS)
	mkdir -p $(INSTALL_DIR)
	cp $(PLUGINS) $(INSTALL_DIR)

clean:
	rm -f $(PLUGINS)
	rm -f *.o

.PHONY: install clean
