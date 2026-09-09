#!/bin/sh
set -e
cd "$(dirname "$0")"
echo "building json_test..."
cc -o json_test src/json_test.c \
   -g -DDEBUG \
   -Wall \
   -Wextra \
   -Werror \
   -Wpointer-arith \
   -Wcast-align \
   -Wunreachable-code \
   -lm
echo "running json_test..."
./json_test -v
