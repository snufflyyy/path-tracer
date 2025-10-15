#pragma once

#include "math/vector3.h"
#include "rayhit.h"
#include "types/base_types.h"
#include "math/ray.h"

typedef struct Sphere {
  Vector3 position;
  f32 radius;
} Sphere;

RayHit sphere_ray_hit(Sphere* sphere, Ray* ray);
