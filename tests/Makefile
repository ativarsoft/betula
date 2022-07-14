TESTS=simple.xhtml library.xhtml placeholder.xhtml
CFLAGS=-O0 -ggdb -I../include
LDFLAGS=-ltemplatizer
EXEC= #cookie percent json

test: $(TESTS) $(EXEC)

simple.xhtml: simple.tmpl
	$(shell PATH_TRANSLATED="simple.tmpl" ../templatizer > $@)
library.xhtml: library.tmpl
	$(shell PATH_TRANSLATED="library.tmpl" ../templatizer > $@)
placeholder.xhtml: placeholder.tmpl
	$(shell PATH_TRANSLATED="placeholder.tmpl" ../templatizer > $@)

clean:
	rm -f $(EXEC) *.o
	rm -f simple.xhtml
	rm -f library.xhtml
	rm -f placeholder.xhtml
