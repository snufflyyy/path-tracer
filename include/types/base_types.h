#pragma once

#include <stddef.h>
#include <stdint.h>

typedef size_t usize;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef float f32;
typedef double f64;

u32 pcg32(u64* state);
f32 random_f32(u64* state);
f32 random_f32_range(u64* state, f32 min, f32 max);
