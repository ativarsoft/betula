#!/bin/bash -e

get_valgrind()
{
  if [ "$TERMUX" == "y" ]
  then
    echo ""
    return
  fi
  echo $(which valgrind)
}

get_valgrind | read VALGRIND

verify_checksum()
{
  if test "$1" == ""
  then
    return
  fi
  local file="$1"
  local md5file="$file.md5"
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

PATH_TRANSLATED="$1" $VALGRIND ../templatizer &> $2
verify_checksum "$2"
PATH_TRANSLATED="$(pwd)/$1" $VALGRIND ../templatizer &> $2
verify_checksum "$2"
PATH_TRANSLATED="$1" DOCUMENT_ROOT="$(pwd)" $VALGRIND ../templatizer &> $2
verify_checksum "$2"
