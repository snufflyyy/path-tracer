#pragma once

#include <cJSON.h>

#include "hittables/hittable.h"
#include "math/vector3.h"
#include "math/vector2.h"

typedef struct HittablePlane {
  Hittable hittable;

  Vector3 position;
  Vector3 normal;
  Vector3 right;
  Vector3 up;

  Vector2 size;
} HittablePlane;

HittablePlane* hittable_plane_create(Vector3 position, Vector3 normal, Vector2 size, Material* material);
void hittable_plane_update_tangent_vectors(HittablePlane* plane);
RayHit hittable_plane_ray_hit(HittablePlane* plane, Ray ray);

cJSON* hittable_plane_json_create(HittablePlane* plane);
HittablePlane* hittable_plane_json_parse(cJSON* plane_json);

void hittable_plane_destroy(HittablePlane* plane);
