#ifndef GENERAL_H
#define GENERAL_H

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef float  f32;
typedef double f64;

#define KILOBYTES(x) (1024*x)
#define MEGABYTES(x) (KILOBYTES(x))
#define GIGABYTES(x) (GIGABYTES(x))

#endif // GENERAL_H
