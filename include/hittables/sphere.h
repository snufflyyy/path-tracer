#pragma once

#include <cJSON.h>

#include "hittables/hittable.h"
#include "math/vector3.h"
#include "types/base_types.h"
#include "materials/material.h"
#include "types/rayhit.h"

typedef struct HittableSphere {
  Hittable hittable;

  Vector3 position;
  f32 radius;
} HittableSphere;

HittableSphere* hittable_sphere_create(Vector3 position, f32 radius, Material* material);
RayHit hittable_sphere_ray_hit(HittableSphere* sphere, Ray ray);

cJSON* hittable_sphere_json_create(HittableSphere* sphere);
HittableSphere* hittable_sphere_json_parse(cJSON* sphere_json);

void hittable_sphere_destroy(HittableSphere* sphere);
