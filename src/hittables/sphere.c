#include "hittables/sphere.h"

#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#include "hittables/hittable.h"
#include "types/material.h"
#include "math/ray.h"
#include "math/vector3.h"
#include "rayhit.h"

RayHit sphere_ray_hit(Hittable* hittable, Ray ray);

Sphere* sphere_create(Vector3 position, f32 radius, Material material) {
  Sphere* sphere = (Sphere*) malloc(sizeof(Sphere));

  sphere->hittable.position = &sphere->position;
  sphere->hittable.hit = sphere_ray_hit;

  sphere->position = position;
  sphere->radius = radius;

  sphere->material = material;

  return sphere;
}

RayHit sphere_ray_hit(Hittable* hittable, Ray ray) {
  Sphere* sphere = (Sphere*) hittable;

  Vector3 oc = vector3_subtract(sphere->position, ray.origin);
  f32 a = vector3_length_squared(ray.direction);
  f32 h = vector3_dot_product(ray.direction, oc);
  f32 c = vector3_length_squared(oc) - (sphere->radius * sphere->radius);
  f32 discriminant = (h * h) - (a * c);

  if (discriminant < 0.0f) {
    return (RayHit) {0};
  }

  f32 t = (h - sqrtf(discriminant)) / a;
  Vector3 hit_position = ray_at(ray, t);

  RayHit rayhit = {
    .hit = true,
    .t = t,
    .hit_position = hit_position,
    .normal = vector3_normalize(vector3_subtract(hit_position, sphere->position)),
    .material = sphere->material
  };

  return rayhit;
}
