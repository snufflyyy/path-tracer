#pragma once

#include "types/base_types.h"

typedef struct Vector3 {
  f32 x, y, z;
} Vector3;

Vector3 vector3_add(Vector3 a, Vector3 b);
Vector3 vector3_subtract(Vector3 a, Vector3 b);
Vector3 vector3_scale(Vector3 a, f32 scalar);

Vector3 vector3_normalize(Vector3 a);
Vector3 vector3_reflect(Vector3 a, Vector3 normal);
Vector3 vector3_refract(Vector3 a, Vector3 normal, f32 refraction_ratio);

f32 vector3_length(Vector3 a);
f32 vector3_length_squared(Vector3 a);
f32 vector3_dot_product(Vector3 a, Vector3 b);
