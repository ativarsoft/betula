#MATH_RS=build/libmath_rs.a
MATH_RS=target/release/libmath_rs.a

LIBS=$(MATH_RS)

all: $(LIBS)

$(MATH_RS): src/lib.rs
ifneq ($(shell which cargo),)
	cargo build --release #--out-dir="build/" -Z unstable-options
endif

install:

clean:
ifneq ($(shell which cargo),)
	cargo clean
endif
	rm -fr build/

.PHONY: clean install

