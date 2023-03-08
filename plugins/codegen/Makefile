LLVM_CFLAGS=$(shell llvm-config --cflags)
LLVM_LDFLAGS=$(shell llvm-config --ldflags)
LLVM_LIBS=$(shell llvm-config --libs)

CFLAGS=-Wall -Wextra -fPIC -I../../include/
LDFLAGS=-Wl,--no-as-needed
PLUGINS=llvm.so cd_i386.so codegen_c.so

PREFIX?=/usr
INSTALL_DIR=$(PREFIX)/lib/pollen/plugins

all: $(PLUGINS)

cd_i386.so: cd_i386.o
	$(CC) $(CFLAGS) --shared -o $@ $^

codegen_c.so: codegen_c.o
	$(CC) $(CFLAGS) --shared -o $@ $^

llvm.o: llvm.c
	$(CC) -c $(CFLAGS) $(LLVM_CFLAGS) $(LLVM_LDFLAGS) -o $@ $^ $(LLVM_LIBS)

llvm.so: llvm.o
	$(CC) $(CFLAGS) $(LLVM_CFLAGS) $(LDFLAGS) $(LLVM_LDFLAGS) --shared -o $@ $^ $(LLVM_LIBS)

install:
	mkdir -p $(INSTALL_DIR)
	cp $(PLUGINS) $(INSTALL_DIR)

clean:
	rm -f $(PLUGINS) *.o

.PHONY: install clean
