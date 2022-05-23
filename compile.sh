#!/bin/bash

CC="clang"

CFLAGS="-std=c99 -c"
DFLAGS="-Wall -g"
LFLAGS="-lX11 -ldl -lGL -lm"
IFLAGS="-I ./src/external"

SOURCE_MAIN="src/main.c"
SOURCE_PLATFORM="src/platform/lin64/lin64_fchat.c"
SOURCE_RENDERER="src/renderer/renderer.c"
SOURCE_STB_TRUETYPE="src/external/stb_truetype.c"

$CC $CFLAGS $DFLAGS $SOURCE_MAIN -o build/main.o
$CC $CFLAGS $DFLAGS $IFLAGS $SOURCE_RENDERER -o build/renderer.o
$CC $CFLAGS $DFLAGS $SOURCE_PLATFORM -o build/platform.o
$CC $CFLAGS $DFLAGS $SOURCE_STB_TRUETYPE -o build/stb_truetype.o

$CC $DFLAGS build/*.o $LFLAGS -o build/fchat
#ld build/*.o $LD_FLAGS -o build/client
