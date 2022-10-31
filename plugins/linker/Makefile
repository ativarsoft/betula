CFLAGS=-Wall -Wextra -fpic

all: ld-ada.so

ld-ada.so: ld-ada.o
	$(CC) $(CFLAGS) -shared -o$@ $^ -ldl

install:

clean:
	rm -f *.so *.o

.PHONY: install clean
