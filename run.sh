#!/bin/sh

DIR=`dirname $0`

platform='unknown'
unamestr=`uname`

if [ "$unamestr" = "Linux" ]; then
   platform='linux'
elif [ "$unamestr" = 'Darwin' ]; then
   platform='osx'
fi

if [ $platform = 'linux' ]; then
  export LD_LIBRARY_PATH=$DIR/lib:$LD_LIBRARY_PATH
elif [ $platform = 'osx' ]; then
  export DYLD_LIBRARY_PATH=$DIR/lib:$DYLD_LIBRARY_PATH
else
  echo "Could not determine the OS type"
  exit 1
fi

$DIR/login $@ $PARSE_APPLICATION_ID $PARSE_CLIENT_KEY

