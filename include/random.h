#pragma once

#include "types/base_types.h"
#include "math/vector3.h"

u32 pcg32(u64* state);
f32 random_f32(u64* state);
f32 random_f32_range(u64* state, f32 min, f32 max);

Vector3 vector3_random(u64* state, f32 min, f32 max);
Vector3 vector3_random_unit_vector(u64* state);
Vector3 vector3_random_in_hemisphere(u64* state, Vector3 normal);
