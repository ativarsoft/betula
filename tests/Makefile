TESTS=simple.xhtml library.xhtml placeholder.xhtml
CFLAGS=-O0 -ggdb
LDFLAGS=-ltemplatizer
EXEC=cookie percent

all: $(TESTS) $(EXEC)

simple.xhtml:
	$(shell PATH_TRANSLATED="simple.tmpl" ../templatizer > $@)
library.xhtml:
	$(shell PATH_TRANSLATED="library.tmpl" ../templatizer > $@)
placeholder.xhtml:
	$(shell PATH_TRANSLATED="placeholder.tmpl" ../templatizer > $@)

cookie: cookie.o

clean:
	rm -f $(EXEC) *.o
