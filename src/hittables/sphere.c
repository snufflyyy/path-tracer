#include "hittables/sphere.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <cJSON.h>

#include "types/rayhit.h"
#include "hittables/hittable.h"
#include "math/ray.h"
#include "types/base_types.h"
#include "materials/material.h"
#include "math/vector3.h"

static RayHit hit(Hittable* hittable, Ray ray);
static void destroy(Hittable* hittable);

HittableSphere* hittable_sphere_create(Vector3 position, f32 radius, Material* material) {
  HittableSphere* sphere = (HittableSphere*) malloc(sizeof(HittableSphere));
  if (!sphere) {
    fprintf(stderr, "[ERROR] [HITTABLE] [SPHERE] Failed to allocate memory for sphere!\n");
    return NULL;
  }

  sphere->hittable = (Hittable) {
    .type = HITTABLE_TYPE_SPHERE,
    .identifer = "Sphere",
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
  return hittable_sphere_ray_hit((HittableSphere*) hittable, ray);
}

inline static void destroy(Hittable* hittable) {
  hittable_sphere_destroy((HittableSphere*) hittable);
}

RayHit hittable_sphere_ray_hit(HittableSphere* sphere, Ray ray) {
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

  f32 theta = acosf(-normal.y);
  f32 phi = atan2f(-normal.z, normal.x) + M_PI;

  Vector2 uv_coordinates = { phi / (2.0f * M_PI), theta / M_PI };

  RayHit rayhit = {
    .hit = true,
    .ray = ray,
    .t = t,
    .hit_position = hit_position,
    .normal = vector3_normalize(vector3_subtract(hit_position, sphere->position)),
    .uv_coordinates = uv_coordinates,
    .inside = inside,
    .material = sphere->hittable.material
  };

  return rayhit;
}

cJSON* hittable_sphere_json_create(HittableSphere* sphere) {
  cJSON* hittable = cJSON_CreateObject();
  if (!hittable) { goto error; }

  if (!cJSON_AddNumberToObject(hittable, "type", HITTABLE_TYPE_SPHERE)) { goto error; }

  cJSON* position = cJSON_AddArrayToObject(hittable, "position");
  if (!position) { goto error; }

  for (usize i = 0; i < 3; i++) {
    cJSON* element = cJSON_CreateNumber(sphere->position.data[i]);
    if (!element) { goto error; }

    if (!cJSON_AddItemToArray(position, element)) { goto error; }
  }

  if (!cJSON_AddNumberToObject(hittable, "radius", sphere->radius)) { goto error; }

  return hittable;

error:
  fprintf(stderr, "[ERROR] [HITTABLE] [SPHERE] [JSON] Failed to create JSON object!\n");
  cJSON_Delete(hittable);
  return NULL;
}

HittableSphere* hittable_sphere_json_parse(cJSON* sphere_json) {
  cJSON* position = cJSON_GetObjectItemCaseSensitive(sphere_json, "position");
  if (!position || !cJSON_IsArray(position)) { goto error; }

  cJSON* radius = cJSON_GetObjectItemCaseSensitive(sphere_json, "radius");
  if (!radius || !cJSON_IsNumber(radius)) { goto error; }

  Vector3 new_position = { cJSON_GetNumberValue(cJSON_GetArrayItem(position, 0)), cJSON_GetNumberValue(cJSON_GetArrayItem(position, 1)), cJSON_GetNumberValue(cJSON_GetArrayItem(position, 2)) };
  return hittable_sphere_create(new_position, cJSON_GetNumberValue(radius), NULL);

error:
  fprintf(stderr, "[ERROR] [HITTABLE] [SPHERE] [JSON] Failed to parse JSON object!\n");
  return NULL;
}

void hittable_sphere_destroy(HittableSphere* sphere) {
  free(sphere->hittable.material);
  free(sphere);
}
