#!/bin/sh

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export DYLD_LIBRARY_PATH=$DIR/lib:$DYLD_LIBRARY_PATH

$DIR/login $@ $PARSE_APPLICATION_ID $PARSE_CLIENT_KEY

