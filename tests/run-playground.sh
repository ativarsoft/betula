#!/bin/bash -e

export CONTENT_LENGTH=$(wc -c playground.txt | awk '{print $1}')
export PATH_TRANSLATED="playground.tmpl"
export REQUEST_METHOD="POST"
cat playground.txt | ../templatizer > playground.xhtml

