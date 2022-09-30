#!/bin/bash -e

verify_checksum()
{
  file="$1"
  md5file="$file.md5"
  if [ -f "$md5file" ]
  then
    echo "Checking '$md5file'."
    md5sum -c "$md5file"
  else
    echo "Creating '$md5file'."
    md5sum "$file" > "$md5file"
  fi
  return $?
}

PATH_TRANSLATED="$1" ../templatizer > $2
verify_checksum "$2"
PATH_TRANSLATED="$(pwd)/$1" ../templatizer > $2
verify_checksum "$2"
PATH_TRANSLATED="$1" DOCUMENT_ROOT="$(pwd)" ../templatizer > $2
verify_checksum "$2"
