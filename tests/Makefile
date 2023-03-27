# Copyright (C) 2023 Mateus de Lima Oliveira

TESTS=simple.xhtml library.xhtml placeholder.xhtml placeholder-d.xhtml \
  placeholder-rs.xhtml kv.xhtml pipeline1.xhtml selfclosing.xhtml \
  controlflow.xhtml echo.xhtml sanity.xhtml noplugin.xhtml include.xhtml \
  playground.xhtml
CFLAGS=-O0 -ggdb -I../include
LDFLAGS=-ltemplatizer
EXEC= sanity #cookie percent json

../templatizer:	$(error Templatizer executable not found)
l
pollen.conf:
	true $(shell ./create-config.sh)
	test $(.SHELLSTATUS)

test: $(TESTS)

simple.xhtml: simple.tmpl ../templatizer pollen.conf
	true $(shell ./run-templatizer.sh $< $@)
	test $(.SHELLSTATUS)
library.xhtml: library.tmpl ../templatizer pollen.conf
	true $(shell ./run-templatizer.sh $< $@)
	test $(.SHELLSTATUS)
placeholder.xhtml: placeholder.tmpl ../templatizer pollen.conf
	true $(shell ./run-templatizer.sh $< $@)
	test $(.SHELLSTATUS)
placeholder-d.xhtml: placeholder-d.tmpl ../templatizer pollen.conf
	true $(shell ./run-templatizer.sh $< $@)
	test $(.SHELLSTATUS)
placeholder-rs.xhtml: placeholder-rs.tmpl ../templatizer pollen.conf
	true $(shell ./run-templatizer.sh $< $@)
	test $(.SHELLSTATUS)
kv.xhtml: kv.tmpl ../templatizer pollen.conf
	true $(shell ./run-templatizer.sh $< $@)
	test $(.SHELLSTATUS)
pipeline1.xhtml: pipeline1.tmpl ../templatizer pollen.conf
	true $(shell ./run-templatizer.sh $< $@)
	test $(.SHELLSTATUS)
selfclosing.xhtml: selfclosing.tmpl ../templatizer pollen.conf
	true $(shell  ./run-templatizer.sh $< $@)
	test $(.SHELLSTATUS)
controlflow.xhtml: controlflow.tmpl ../templatizer pollen.conf
	true $(shell ./run-templatizer.sh $< $@)
	test $(.SHELLSTATUS)
echo.xhtml: echo.tmpl ../templatizer pollen.conf
	true $(shell ./run-templatizer.sh $< $@)
	test $(.SHELLSTATUS)
sanity.xhtml: sanity.tmpl ../templatizer pollen.conf
	true $(shell ./run-templatizer.sh $< $@)
	test $(.SHELLSTATUS)
noplugin.xhtml: noplugin.tmpl ../templatizer pollen.conf
	true $(shell  ./run-templatizer.sh $< $@)
	test $(.SHELLSTATUS)
include.xhtml: include.tmpl ../templatizer pollen.conf
	true $(shell ./run-templatizer.sh $< $@)
	test $(.SHELLSTATUS)
playground.xhtml: playground.tmpl ../templatizer pollen.conf
	true $(shell ./run-playground.sh)
	test $(.SHELLSTATUS)

clean:
	rm -f $(EXEC) *.o *.md5
	rm -f $(TESTS)
	rm -f access.log
	rm -f pollen.conf
	rm -f sanity
	rm -f /tmp/playground-test-*.tmpl
	rm -f playground-test-*.tmpl

.PHONY: clean
