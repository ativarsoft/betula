# Compiler to use
CC=gcc

# Compiler flags
CFLAGS=-Wall -Wextra -pedantic -std=c11 -I../include

# Source files
SRC=yeast.c

# Object files
OBJ=$(SRC:.c=.o)

# Library name
LIBNAME=libyeast.a

all: $(LIBNAME)

# Build the library from the object files
$(LIBNAME): $(OBJ)
	ar rcs $(LIBNAME) $(OBJ)

# Compile the source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(LIBNAME)
