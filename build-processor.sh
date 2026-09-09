#!/bin/sh

echo "building with debug symbols..."
cc -o processor src/processor.c \
   -g -DDEBUG \
   -Wall \
   -Wextra \
   -Werror \
   -Wpointer-arith \
   -Wcast-align \
   -Wunreachable-code \
   -lm

