TESTS=simple.xhtml library.xhtml placeholder.xhtml kv.xhtml
CFLAGS=-O0 -ggdb -I../include
LDFLAGS=-ltemplatizer
EXEC= #cookie percent json

test: $(TESTS) $(EXEC)

simple.xhtml: simple.tmpl ../templatizer
	./run-templatizer.sh $< $@
library.xhtml: library.tmpl ../templatizer
	./run-templatizer.sh $< $@
placeholder.xhtml: placeholder.tmpl ../templatizer
	./run-templatizer.sh $< $@
kv.xhtml: kv.tmpl ../templatizer
	./run-templatizer.sh $< $@

clean:
	rm -f $(EXEC) *.o *.md5
	rm -f simple.xhtml
	rm -f library.xhtml
	rm -f placeholder.xhtml
	rm -f kv.xhtml
	rm -f access.log

.PHONY: clean
