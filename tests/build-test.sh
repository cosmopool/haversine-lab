#!/bin/sh
set -e
cd "$(dirname "$0")"
printf "building json_test..."
cc -o json_test src/json_test.c \
   -g -O0 -fno-omit-frame-pointer -DDEBUG \
   -Wall \
   -Wextra \
   -Werror \
   -Wpointer-arith \
   -Wcast-align \
   -Wunreachable-code \
   -lm

if [ "$1" = "--run" ]; then
  printf "running json_test...\n"
  ./json_test -v
else
  printf " success!\n"
  echo "to also run the tests, use: '$0 --run'"
fi
