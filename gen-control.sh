#!/bin/bash -e

version=$(./version.sh)

gen() {
    echo "Package: pollen"
    echo "Version: $version"
    echo "Maintainer: Mateus de Lima Oliveira"
    echo "Architecture: amd64"
    echo "Description: A XML/HTML templating programming language."
}

gen > debian/control
