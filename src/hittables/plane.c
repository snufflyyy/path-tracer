#include "hittables/plane.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "hittables/hittable.h"
#include "math/vector3.h"
#include "math/vector2.h"

static RayHit hit(Hittable* hittable, Ray ray);
static void destroy(Hittable* hittable);

Plane* hittable_plane_create(Vector3 position, Vector3 normal, Vector2 size, Material* material) {
  Plane* plane = (Plane*) malloc(sizeof(Plane));
  if (!plane) {
    fprintf(stderr, "[ERROR] [HITTABLE] [PLANE] Failed to allocate memory for plane!\n");
    return NULL;
  }

  plane->hittable = (Hittable) {
    .type = PLANE,
    .identifer = "Plane",
    .position = &plane->position,
    .material = material,
    .hit = hit,
    .destroy = destroy
  };

  plane->position = position;
  plane->normal = normal;
  hittable_plane_update_tangent_vectors(plane);
  plane->size = size;

  return plane;
}

static inline RayHit hit(Hittable* hittable, Ray ray) {
  return hittable_plane_ray_hit((Plane*) hittable, ray);
}

static inline void destroy(Hittable* hittable) {
  hittable_plane_destroy((Plane*) hittable);
}

void hittable_plane_update_tangent_vectors(Plane* plane) {
  Vector3 world_up = { 0.0f, 1.0f, 0.0f };

  if (fabs(vector3_dot_product(plane->normal, world_up)) > 0.999f) {
    world_up = (Vector3) { 1.0f, 0.0f, 0.0f };
  }

  plane->right = vector3_normalize(vector3_cross_product(world_up, plane->normal));
  plane->up = vector3_cross_product(plane->normal, plane->right);
}

RayHit hittable_plane_ray_hit(Plane* plane, Ray ray) {
  if (vector3_dot_product(ray.direction, plane->normal) > 0.0f) { return (RayHit) {0}; }

  f32 denomanator = vector3_dot_product(ray.direction, plane->normal);
  f32 t = vector3_dot_product(vector3_subtract(plane->position, ray.origin), plane->normal) / denomanator;
  if (t < 0.0f) { return (RayHit) {0}; }

  Vector3 hit_position = ray_at(ray, t);
  Vector3 v = vector3_subtract(hit_position, plane->position);

  f32 u = vector3_dot_product(v, plane->right);
  f32 vcoord = vector3_dot_product(v, plane->up);

  if (fabs(u) > (plane->size.x / 2.0f) || fabs(vcoord) > (plane->size.y / 1.0f)) { return (RayHit) {0}; }

  RayHit rayhit = (RayHit) {
    .hit = true,
    .ray = ray,
    .t = t,
    .hit_position = hit_position,
    .normal = plane->normal,
    .inside = false,
    .material = plane->hittable.material
  };

  return rayhit;
}

void hittable_plane_destroy(Plane* plane) {
  free(plane->hittable.material);
  free(plane);
}
