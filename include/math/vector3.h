#pragma once

#include "types/base_types.h"

typedef union Vector3 {
  struct { f32 x, y, z, w; };
  f32 data[4];
} Vector3;

Vector3 vector3_add(Vector3 a, Vector3 b);
Vector3 vector3_subtract(Vector3 a, Vector3 b);
Vector3 vector3_scale(Vector3 a, f32 scalar);

Vector3 vector3_normalize(Vector3 a);
Vector3 vector3_reflect(Vector3 a, Vector3 normal);
Vector3 vector3_refract(Vector3 a, Vector3 normal, f32 refraction_ratio);
Vector3 vector3_cross_product(Vector3 a, Vector3 b);

f32 vector3_length(Vector3 a);
f32 vector3_length_squared(Vector3 a);
f32 vector3_dot_product(Vector3 a, Vector3 b);
