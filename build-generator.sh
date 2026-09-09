#!/bin/sh

echo "cleaning old files..."
rm *.bin
rm *.json

echo "building with debug symbols..."
cc -o generator src/generator.c \
   -g -DDEBUG \
   -Wall \
   -Wextra \
   -Werror \
   -Wpointer-arith \
   -Wcast-align \
   -Wunreachable-code \
   -lm

