MATH_RS=build/libmath_rs.a

LIBS=$(MATH_RS)

all: $(LIBS)

$(MATH_RS): src/lib.rs
	cargo build --release --out-dir="build/" -Z unstable-options

install:

clean:
	cargo clean
	rm -fr build/

.PHONY: clean install

