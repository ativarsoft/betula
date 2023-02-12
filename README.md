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

In computing, a
[plug-in](https://en.wikipedia.org/wiki/Plug-in_(computing))
(or plugin, add-in, addin, add-on, or addon) is a software
component that adds a specific feature to an existing
computer program. When a program supports plug-ins, it
enables customization.

Input plugins read
[url-encoded](https://en.wikipedia.org/wiki/URL_encoding)
and [JSON](https://en.wikipedia.org/wiki/JSON)
input from the client. Those values are then used in a
script using the '@' symbol. Input plugins can be selected
using the 'templatizer' tag.

[Dynamic Linker](https://en.wikipedia.org/wiki/Dynamic_linker)
plugins can be used to load programs written in
specific programming languages. The plugin is specified
using the 'lib' attribute on the 'templatizer' element.
The template can set plugin arguments using the 'args'
XML attribute.

A linker called [ld-exec.so](https://www.wikidata.org/wiki/Q47513204)
can be used to run binary
programs or text scripts that begin with a shebang on
Linux.

Some plugins are full web apps. Text placeholder variables
are set using variable names and those variables can be used
on XHTML template placeholders. Different websites can have
different user interfaces while using the same Templatier
app plugin.

## Example

The following is an example of a program written in Templatizer.
Note that the Templatizer syntax this example as well as other
Templatizer programs were intentionaly made to be similar to
XML syntax.

```xml
<!-- Copyright (C) 2023 Mateus de Lima Oliveira -->
<templatizer lib="../plugins/input/in_helloworld.so">
<templatizer lib="../plugins/input/helloworld-rs/target/release/libhelloworld_rs.so">
<templatizer lib="../plugins/net/headers.so">
<html>
  <head>
    <meta charset="UTF-8"/>
    <title>Hello world pipeline</title>
  </head>
  <body>
    <h1>Hello world pipeline</h1>
    <p>Hello world Templatizer pipeline in multiple programming languages:</p>
    <ul>
      <li>C: <span>@</span></li>
      <li>Rust: <span>@</span></li>
    </ul>
    <p>Have a nice day!</p>
    <templatizer lib="../plugins/net/copyright.so">
    <p>@</p>
    </templatizer>
  </body>
</html>
</templatizer>
</templatizer>
</templatizer>
```

More tests and examples can be found in the tests directory
of this source tree.

## Compiling from source

Templatizer was only tested on Linux.

It requires:

  * [sys/queue.h](https://man.freebsd.org/cgi/man.cgi?query=queue&sektion=3) for list macros (installed by default on most Linux distributions);
  * [libdl](https://refspecs.linuxfoundation.org/LSB_3.0.0/LSB-Core-IA64/LSB-Core-IA64/libdl.html) for dynamically loading libraries (installed by default on most Linux distributions);
  * [build-essential](https://packages.debian.org/bullseye/build-essential)
  * [libexpat1-dev](https://packages.debian.org/bullseye/libexpat1-dev);
  * [flex](https://packages.debian.org/bullseye/flex) and [bison](https://packages.debian.org/bullseye/bison);
  * [libapr1-dev](https://packages.debian.org/bullseye/libapr1-dev);
  * [libvirt-dev](https://packages.debian.org/bullseye/libvirt-dev);
  * [liblmdb-dev](https://packages.debian.org/bullseye/liblmdb-dev);
  * [ldc](https://packages.debian.org/bullseye/ldc);
  * [cargo](https://packages.debian.org/bullseye/cargo);
  * [rustc](https://packages.debian.org/bullseye/rustc);
  * [valgrind](https://packages.debian.org/bullseye/valgrind).

```console
root@debian:~# make dependencies
mateus@debian:~$ make
mateus@debian:~$ make test
root@debian:~# make install
```

### Termux

Compile Templatizer for termux with:

```console
mateus@debian:~$ make termux
root@debian:~# make install
```

## Contributing

If you like this project, please consider making
a donation to help me develop it further. :)
