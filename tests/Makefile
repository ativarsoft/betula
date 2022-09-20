TESTS=simple.xhtml library.xhtml placeholder.xhtml kv.xhtml
CFLAGS=-O0 -ggdb -I../include
LDFLAGS=-ltemplatizer
EXEC= #cookie percent json

test: $(TESTS) $(EXEC)

simple.xhtml: simple.tmpl ../templatizer
	$(shell PATH_TRANSLATED="simple.tmpl" ../templatizer > $@)
library.xhtml: library.tmpl ../templatizer
	$(shell PATH_TRANSLATED="library.tmpl" ../templatizer > $@)
placeholder.xhtml: placeholder.tmpl ../templatizer
	$(shell PATH_TRANSLATED="placeholder.tmpl" ../templatizer > $@)
kv.xhtml: kv.tmpl ../templatizer
	$(shell PATH_TRANSLATED="kv.tmpl" ../templatizer > $@)

clean:
	rm -f $(EXEC) *.o
	rm -f simple.xhtml
	rm -f library.xhtml
	rm -f placeholder.xhtml
	rm -f kv.xhtml
	rm -f access.log

.PHONY: clean
