#pragma once

#include "types/base_types.h"
#include "math/vector3.h"
#include "hittables/hittable.h"

u32 pcg32(u64* state);
f32 random_f32(u64* state);
f32 random_f32_range(u64* state, f32 min, f32 max);

Vector3 random_vector3(u64* state, f32 min, f32 max);
Vector3 random_vector3_unit_vector(u64* state);
Vector3 random_vector3_in_hemisphere(u64* state, Vector3 normal);

Vector3 random_hittable_position(u64* state, Hittable* hittable);
