CFLAGS=-Wall -Wextra -fPIC -I../../include
PLUGINS=ld-ada.so ld-exec.so

all: $(PLUGINS)

ld-ada.so: ld-ada.o
	$(CC) $(CFLAGS) -shared -o$@ $^ -ldl

ld-exec.so: ld-exec.o
	$(CC) $(CFLAGS) -shared -o$@ $^

install: $(PLUGINS)

clean:
	rm -f $(PLUGINS)
	rm -f *.o

.PHONY: install clean
