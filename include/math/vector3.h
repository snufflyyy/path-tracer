#pragma once

#include "types/base_types.h"

typedef struct Vector3 {
  f32 x, y, z;
} Vector3;

Vector3 vector3_add(Vector3 a, Vector3 b);
Vector3 vector3_subtract(Vector3 a, Vector3 b);
Vector3 vector3_scale(Vector3 a, f32 scalar);

f32 vector3_dot_product(Vector3 a, Vector3 b);
