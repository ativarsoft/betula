# Templatizer

A XML/HTML templating programming language.
Templatizer is similar to Dartmouth BASIC:
scripts are a series of complex instructions provided by
the runtime. New element tags can be added to the
runtime by using general purpose plugins.

An [HTML template](https://en.wikipedia.org/wiki/Web_template_system)
is an HTML model. Templates are
preformatted HTML in which text or image content
can be inserted into. Most modern template languages
have some form of programming in them. Templatizer
also does that but it reuses the XML systax for that,
without having to create a whole new syntax. Therefore
new programmers can feel familiarized with the Templatizer
language right on the beggining of learning on how to
code on it.

[XML](https://en.wikipedia.org/wiki/XML) tags represent
the data structure and contain metadata.

Templatizer (previously webutils) was created with the
intent to stick to the UNIX philosophy of
keeping things simple and modular
while staying fast and keeping the memory consumption
low.

The original idea was to create a
[Common Gateway Interface (CGI)](https://en.wikipedia.org/wiki/Common_Gateway_Interface)
executable and an Apache module. Something like PHP. But
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

setuid can be used to make Apache run Templatizer as root.
Templatizer gets the user and password from the CGI environment
flags and use setuid to drop privileges to the specified user.
This can be used to restrict filesystem access without using
fakechroot. fakechroot provides wrappers for common system calls.

libtemplatizer provides common functions which are independent
from the templatizer runtime executable. Plugins use libtemplatizer
for code reuse.

## Plugins

Input plugins read url-encoded and JSON
input from the client. Those values are then used in a
script using the '@' symbol. Input plugins can be selected
using the 'templatizer' tag.

Linker plugins can be used to load programs written in
specific programming languages. The plugin is specified
using the 'lib' attribute on the 'templatizer' element.
The template can set plugin arguments using the 'args'
XML attribute.

A linker called ld-exec.so can be used to run binary
programs or text scripts that begin with a shebang on
Linux.

Some plugins are full web apps. Text placeholder variables
are set using variable names and those variables can be used
on XHTML template placeholders. Different websites can have
different user interfaces while using the same Templatier
app plugin.

## Compiling from source

Templatizer was only tested on Linux.

It requires:

  * sys/queue.h for list macros;
  * libdl for dynamically loading libraries;
  * libexpat1-dev;
  * flex and bison.
  * libapr1-dev
