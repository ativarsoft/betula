#!/bin/bash -e

version=$(./version.sh)

gen() {
    echo "Package: pollen-lang"
    echo "Version: $version"
    echo "Maintainer: Mateus de Lima Oliveira"
    echo "Architecture: amd64"
    echo "Description: A XML/HTML templating programming language."
    echo "Depends: build-essential, flex, bison, libapr1-dev, libexpat1-dev, liblmdb-dev, libvirt-dev, libsqlite3-dev, ldc, cargo, rustc, valgrind, llvm-dev, apache2, xsltproc"
}

gen > debian/control
