# Pollen programmming language

A programming language with XML syntax.
Scripts are a series of complex instructions provided by
the runtime. New element tags can be added to the
runtime by using general purpose plugins.

Static checking is used on the C language for a zero-defect program.

An [HTML template](https://en.wikipedia.org/wiki/Web_template_system)
is an HTML model. Templates are
preformatted HTML in which text or image content
can be inserted into. Most modern template languages
have some form of programming in them. Pollen
also does that but it reuses the XML systax for that,
without having to create a whole new syntax. Therefore
new programmers can feel familiarized with the Pollen
language right on the beggining of learning on how to
code on it.

[XML](https://en.wikipedia.org/wiki/XML) tags represent
the data structure and contain metadata.

The original idea was to create a
[Common Gateway Interface (CGI)](https://en.wikipedia.org/wiki/Common_Gateway_Interface)
executable and an Apache module. Something like PHP. But
instead of interpreting a script, Pollen
would load a compiled dynamic library. It provides
a simple templating engine while keeping the speed
of compiled languagues.

The [Apache Runtime Library (APR)](https://apr.apache.org)
is being added to ensure good security.
APR provides dynamic arrays and memory pools to
mitigate use-after-free, buffer overflows and buffer underflows.
A good explanation can be found
[here](http://www.apachetutor.org/dev/pools).

libpollen provides common functions which are independent
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

The following is an example of a program written in Pollen.
It prints [hello world](https://en.wikipedia.org/wiki/%22Hello,_World!%22_program)
comming from two Pollen plugins written in different languages.
Note that the Pollen syntax this example as well as other
Pollen programs were intentionaly made to be similar to
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

Pollen was only tested on Linux.

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
  * [valgrind](https://packages.debian.org/bullseye/valgrind);
  * [llvm-dev](https://packages.debian.org/bullseye/devel/llvm-dev).

```console
root@debian:~# make dependencies
mateus@debian:~$ make
mateus@debian:~$ make test
root@debian:~# make install
```

### Termux

Compile Pollen for termux with:

```console
mateus@debian:~$ make termux
root@debian:~# make install
```

### Debug

Enabling debug can be done by
setting the DEBUG environment
variable to "y":

```console
mateus@debian:~$ DEBUG=y make
```

## Development roadmap

The following is my future development:

  - Convert Quick to a series daemons
  - Remove autotools from Quick
  - Merge Quick with Pollen
  - Fix bugs

## Contributing

If you like this project, please consider making
a donation to help me develop it further. :)
