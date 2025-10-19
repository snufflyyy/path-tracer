#include "hittables/sphere.h"

#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "hittables/hittable.h"
#include "materials/material.h"
#include "math/ray.h"
#include "math/vector3.h"
#include "types/rayhit.h"

static RayHit hit(Hittable* hittable, Ray ray);
static void destroy(Hittable* hittable);

Sphere* sphere_create(Vector3 position, f32 radius, Material* material) {
  Sphere* sphere = (Sphere*) malloc(sizeof(Sphere));
  if (!sphere) {
    fprintf(stderr, "[ERROR] [HITTABLE] [SPHERE] Failed to allocate memory for sphere!\n");
    return NULL;
  }

  sphere->hittable = (Hittable) {
    .position = &sphere->position,
    .material = material,
    .hit = hit,
    .destroy = destroy
  };

  sphere->position = position;
  sphere->radius = radius;

  return sphere;
}

inline static RayHit hit(Hittable* hittable, Ray ray) {
  return sphere_ray_hit((Sphere*) hittable, ray);
}

inline static void destroy(Hittable* hittable) {
  sphere_destroy((Sphere*) hittable);
}

RayHit sphere_ray_hit(Sphere* sphere, Ray ray) {
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
  Vector3 normal = vector3_normalize(vector3_subtract(hit_position, sphere->position));

  bool inside = false;
  if (vector3_dot_product(ray.direction, normal) > 0.0f) {
    inside = true;
    normal = vector3_scale(normal, -1.0f);
  }

  RayHit rayhit = {
    .hit = true,
    .ray = ray,
    .t = t,
    .hit_position = hit_position,
    .normal = vector3_normalize(vector3_subtract(hit_position, sphere->position)),
    .inside = inside,
    .material = sphere->hittable.material
  };

  return rayhit;
}

void sphere_destroy(Sphere* sphere) {
  free(sphere->hittable.material);
  free(sphere);
}
