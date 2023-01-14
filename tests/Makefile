# Copyright (C) 2023 Mateus de Lima Oliveira

TESTS=simple.xhtml library.xhtml placeholder.xhtml placeholder-d.xhtml placeholder-rs.xhtml kv.xhtml pipeline1.xhtml selfclosing.xhtml controlflow.xhtml
CFLAGS=-O0 -ggdb -I../include
LDFLAGS=-ltemplatizer
EXEC= #cookie percent json

../templatizer:
	$(error Templatizer executable not found)

test: $(TESTS) $(EXEC)

simple.xhtml: simple.tmpl ../templatizer
	./run-templatizer.sh $< $@
library.xhtml: library.tmpl ../templatizer
	./run-templatizer.sh $< $@
placeholder.xhtml: placeholder.tmpl ../templatizer
	./run-templatizer.sh $< $@
placeholder-d.xhtml: placeholder-d.tmpl ../templatizer
	./run-templatizer.sh $< $@
placeholder-rs.xhtml: placeholder-rs.tmpl ../templatizer
	./run-templatizer.sh $< $@
kv.xhtml: kv.tmpl ../templatizer
	./run-templatizer.sh $< $@
pipeline1.xhtml: pipeline1.tmpl ../templatizer
	./run-templatizer.sh $< $@
selfclosing.xhtml: selfclosing.tmpl ../templatizer
	./run-templatizer.sh $< $@
controlflow.xhtml: controlflow.tmpl ../templatizer
	./run-templatizer.sh $< $@

clean:
	rm -f $(EXEC) *.o *.md5
	rm -f $(TESTS)
	rm -f access.log

.PHONY: clean
