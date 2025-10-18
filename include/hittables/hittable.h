#pragma once

#include "math/ray.h"
#include "math/vector3.h"
#include "types/rayhit.h"

typedef struct Hittable {
  Vector3* position;
  RayHit (*hit)(struct Hittable* hittable, Ray ray);
} Hittable;
