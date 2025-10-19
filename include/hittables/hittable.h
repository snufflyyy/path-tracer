#pragma once

#include "math/ray.h"
#include "math/vector3.h"
#include "types/rayhit.h"

typedef struct Hittable {
  Vector3* position;
  Material* material;

  RayHit (*hit)(struct Hittable* hittable, Ray ray);
  void (*destroy)(struct Hittable* hittable);
} Hittable;
