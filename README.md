# Templatizer

A XML/HTML templating programming language.
Templatizer is similar to Dartmouth BASIC:
scripts are a series of complex instructions provided by
the runtime. New element tags can be added to the
runtime by using general purpose plugins.

Templatizer (previously webutils) was created with the
intent to stick to the UNIX philosophy of
keeping things simple and modular
while staying fast and keeping the memory consumption
low.

The original idea was to create a CGI executable
and an Apache module. Something like PHP. But
instead of interpreting a script, Templatizer
would load a compiled dynamic library. It provides
a simple templating engine while keeping the speed
of compiled languagues.

The [Apache Runtime Library (APR)](https://apr.apache.org)
is being added to ensure good security.
APR provides dynamic arrays and memory pools to
mitigate use-after-free, buffer overflows and buffer underflows.
A good explanation can be found
[here](http://www.apachetutor.org/dev/pools).

## Plugins

Input plugins read url-encoded and JSON
input from the client. Those values are then used in a
script using the '@' symbol. Input plugins can be selected
using the 'templatizer' tag.

## Compiling from source

Templatizer was only tested on Linux.

It requires:

  * sys/queue.h for list macros;
  * libdl for dynamically loading libraries;
  * libexpat;
  * flex and bison.
