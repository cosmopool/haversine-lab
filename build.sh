#!/bin/sh

echo "building with debug symbols..."
cc -o generator src/main.c \
   -g -DDEBUG \
   -Wall \
   -Wextra \
   -Werror \
   -Wpointer-arith \
   -Wcast-align \
   -Wunreachable-code

