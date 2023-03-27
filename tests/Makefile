# Copyright (C) 2023 Mateus de Lima Oliveira

TESTS=simple.xhtml library.xhtml placeholder.xhtml placeholder-d.xhtml \
  placeholder-rs.xhtml kv.xhtml pipeline1.xhtml selfclosing.xhtml \
  controlflow.xhtml echo.xhtml sanity.xhtml noplugin.xhtml include.xhtml \
  playground.xhtml
CFLAGS=-O0 -ggdb -I../include
LDFLAGS=-ltemplatizer
EXEC= sanity #cookie percent json

../templatizer:
	$(error Templatizer executable not found)

pollen.conf:
	$(shell ./create-config.sh)

test: $(TESTS)

simple.xhtml: simple.tmpl ../templatizer pollen.conf
	./run-templatizer.sh $< $@
library.xhtml: library.tmpl ../templatizer pollen.conf
	./run-templatizer.sh $< $@
placeholder.xhtml: placeholder.tmpl ../templatizer pollen.conf
	./run-templatizer.sh $< $@
placeholder-d.xhtml: placeholder-d.tmpl ../templatizer pollen.conf
	./run-templatizer.sh $< $@
placeholder-rs.xhtml: placeholder-rs.tmpl ../templatizer pollen.conf
	./run-templatizer.sh $< $@
kv.xhtml: kv.tmpl ../templatizer pollen.conf
	./run-templatizer.sh $< $@
pipeline1.xhtml: pipeline1.tmpl ../templatizer pollen.conf
	./run-templatizer.sh $< $@
selfclosing.xhtml: selfclosing.tmpl ../templatizer pollen.conf
	$(shell  ./run-templatizer.sh $< $@)
controlflow.xhtml: controlflow.tmpl ../templatizer pollen.conf
	./run-templatizer.sh $< $@
echo.xhtml: echo.tmpl ../templatizer pollen.conf
	./run-templatizer.sh $< $@
sanity.xhtml: sanity.tmpl ../templatizer pollen.conf
	./run-templatizer.sh $< $@
noplugin.xhtml: noplugin.tmpl ../templatizer pollen.conf
	$(shell  ./run-templatizer.sh $< $@)
include.xhtml: include.tmpl ../templatizer pollen.conf
	./run-templatizer.sh $< $@
playground.xhtml: playground.tmpl ../templatizer pollen.conf
	./run-playground.sh

clean:
	rm -f $(EXEC) *.o *.md5
	rm -f $(TESTS)
	rm -f access.log
	rm -f pollen.conf
	rm -f sanity
	rm -f /tmp/playground-test-*.tmpl
	rm -f playground-test-*.tmpl

.PHONY: clean
