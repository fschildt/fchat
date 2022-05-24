@echo off

set CC=clang
set CFLAGS=-std=c99 -c
set DFLAGS=-Wall -g
set LFLAGS=-lUser32 -lGdi32 -lOpengl32

set SRC_MAIN=src\main.c
set SRC_RENDERER=src\renderer\renderer.c
set SRC_PLATFORM=src\platform\win32\win32_fchat.c

if NOT EXIST build mkdir build
%CC% %CFLAGS% %DFLAGS% %SRC_MAIN% -o build\main.o
%CC% %CFLAGS% %DFLAGS% %SRC_RENDERER% -o build\renderer.o
%CC% %CFLAGS% %DFLAGS% %SRC_PLATFORM% -o build\platform.o

%CC% %DFLAGS% build\*.o %LFLAGS% -o build\fchat.exe

