#pragma once

#include "math/vector3.h"
#include "types/base_types.h"

typedef struct Ray {
  Vector3 origin;
  Vector3 direction;
} Ray;

Vector3 ray_at(Ray ray, f32 t);
