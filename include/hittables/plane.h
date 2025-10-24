#pragma once

#include "hittables/hittable.h"
#include "math/vector3.h"
#include "math/vector2.h"

typedef struct Plane {
  Hittable hittable;

  Vector3 position;
  Vector3 normal;
  Vector3 right;
  Vector3 up;

  Vector2 size;
} Plane;

Plane* plane_create(Vector3 position, Vector3 normal, Vector2 size, Material* material);
void plane_update_tangent_vectors(Plane* plane);
RayHit plane_ray_hit(Plane* plane, Ray ray);
void plane_destroy(Plane* plane);
