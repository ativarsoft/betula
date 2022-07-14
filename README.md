# Templatizer

A XML/HTML templating programming language.
Templatizer is similar to Dartmouth BASIC:
scripts are a series of complex instructions provided by
the runtime. New element tags can be added to the
runtime by using general purpose plugins.

Templatizer (previously webutils) was created with the
intent to stick to the unix philosophy of
keeping things simple and modular
while staying fast and keeping the memory consumption
low.

## Plugins

Input plugins read url-encoded and JSON
input from the client. Those values are then used in a
script using the '@' symbol. Input plugins can be selected
using the 'templatizer' tag.
