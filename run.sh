#!/bin/sh

DIR=`dirname $0`
export DYLD_LIBRARY_PATH=$DIR/lib:$DYLD_LIBRARY_PATH

$DIR/login $@ $PARSE_APPLICATION_ID $PARSE_CLIENT_KEY

