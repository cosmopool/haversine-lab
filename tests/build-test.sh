#!/bin/sh
set -e
cd "$(dirname "$0")"
echo "building json_test..."
cc -o json_test src/json_test.c \
   -g -O0 -fno-omit-frame-pointer -DDEBUG \
   -Wall \
   -Wextra \
   -Werror \
   -Wpointer-arith \
   -Wcast-align \
   -Wunreachable-code \
   -lm
if [ "$1" = "--build-only" ]; then
  echo "build ok (not running json_test)"
  exit 0
fi
echo "running json_test..."
./json_test -v
