#!/bin/bash

CC="clang"

CFLAGS="-std=c99 -c"
DFLAGS="-Wall -g"
LFLAGS="-lX11 -ldl -lGL -lm"

SRC_MAIN="src/main.c"
SRC_PLATFORM="src/platform/lin64/lin64_fchat.c"
SRC_RENDERER="src/renderer/renderer.c"

mkdir -p build
$CC $CFLAGS $DFLAGS $SRC_MAIN -o build/main.o
$CC $CFLAGS $DFLAGS $SRC_RENDERER -o build/renderer.o
$CC $CFLAGS $DFLAGS $SRC_PLATFORM -o build/platform.o

$CC $DFLAGS build/*.o $LFLAGS -o build/fchat
