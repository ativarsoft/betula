CFLAGS=-Wall -Wpedantic -fPIC -O0 -ggdb -I../../include -L../../libpollen
LDFLAGS=-Wl,--no-as-needed
LIBS=-lpollen
PLUGINS=math.so
MATH_RS=math-rs/build/libmath_rs.a

all: $(PLUGINS)

$(MATH_RS):
	$(MAKE) -C math-rs

math.so: math.o $(MATH_RS)
	$(CC) $(CFLAGS) $(LDFLAGS) --shared -o $@ $^ $(LIBS)

install:
	$(MAKE) -C math-rs install

clean:
	$(MAKE) -C math-rs clean
	rm -f $(PLUGINS) *.o

.PHONY: install clean $(MATH_RS)

