#pragma once

#include "hittables/hittable.h"
#include "math/vector3.h"
#include "types/base_types.h"
#include "materials/material.h"
#include "types/rayhit.h"

typedef struct Sphere {
  Hittable hittable;

  Vector3 position;
  f32 radius;
} Sphere;

Sphere* hittable_sphere_create(Vector3 position, f32 radius, Material* material);
RayHit hittable_sphere_ray_hit(Sphere* sphere, Ray ray);
void hittable_sphere_destroy(Sphere* sphere);
