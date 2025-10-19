#pragma once

#include "materials/material.h"
#include "math/vector3.h"
#include "types/base_types.h"
#include "hittables/hittable.h"

typedef struct Sphere {
  Hittable hittable;

  Vector3 position;
  f32 radius;
} Sphere;

Sphere* sphere_create(Vector3 position, f32 radius, Material* material);
RayHit sphere_ray_hit(Sphere* sphere, Ray ray);
void sphere_destroy(Sphere* sphere);
