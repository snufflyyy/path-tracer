#include "hittables/plane.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "cJSON.h"
#include "hittables/hittable.h"
#include "math/vector3.h"
#include "math/vector2.h"

static RayHit hit(Hittable* hittable, Ray ray);
static void destroy(Hittable* hittable);

HittablePlane* hittable_plane_create(Vector3 position, Vector3 normal, Vector2 size, Material* material) {
  HittablePlane* plane = (HittablePlane*) malloc(sizeof(HittablePlane));
  if (!plane) {
    fprintf(stderr, "[ERROR] [HITTABLE] [PLANE] Failed to allocate memory for plane!\n");
    return NULL;
  }

  plane->hittable = (Hittable) {
    .type = HITTABLE_TYPE_PLANE,
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
  return hittable_plane_ray_hit((HittablePlane*) hittable, ray);
}

static inline void destroy(Hittable* hittable) {
  hittable_plane_destroy((HittablePlane*) hittable);
}

void hittable_plane_update_tangent_vectors(HittablePlane* plane) {
  Vector3 world_up = { 0.0f, 1.0f, 0.0f };

  if (fabs(vector3_dot_product(plane->normal, world_up)) > 0.999f) {
    world_up = (Vector3) { 1.0f, 0.0f, 0.0f };
  }

  plane->right = vector3_normalize(vector3_cross_product(world_up, plane->normal));
  plane->up = vector3_cross_product(plane->normal, plane->right);
}

RayHit hittable_plane_ray_hit(HittablePlane* plane, Ray ray) {
  if (vector3_dot_product(ray.direction, plane->normal) > 0.0f) { return (RayHit) {0}; }

  f32 denomanator = vector3_dot_product(ray.direction, plane->normal);
  f32 t = vector3_dot_product(vector3_subtract(plane->position, ray.origin), plane->normal) / denomanator;
  if (t < 0.0f) { return (RayHit) {0}; }

  Vector3 hit_position = ray_at(ray, t);
  Vector3 v = vector3_subtract(hit_position, plane->position);

  f32 u = vector3_dot_product(v, plane->right);
  f32 vcoord = vector3_dot_product(v, plane->up);

  if (fabs(u) > (plane->size.x / 2.0f) || fabs(vcoord) > (plane->size.y / 2.0f)) { return (RayHit) {0}; }

  RayHit rayhit = (RayHit) {
    .hit = true,
    .ray = ray,
    .t = t,
    .uv_coordinates = (Vector2) { u, vcoord },
    .hit_position = hit_position,
    .normal = plane->normal,
    .inside = false,
    .material = plane->hittable.material
  };

  return rayhit;
}

cJSON* hittable_plane_json_create(HittablePlane* plane) {
  cJSON* hittable = cJSON_CreateObject();
  if (!hittable) { goto error; }

  if (!cJSON_AddNumberToObject(hittable, "type", HITTABLE_TYPE_PLANE)) { goto error; }

  cJSON* position = cJSON_AddArrayToObject(hittable, "position");
  if (!position) { goto error; }

  for (usize i = 0; i < 3; i++) {
    cJSON* element = cJSON_CreateNumber(plane->position.data[i]);
    if (!element) { goto error; }

    if (!cJSON_AddItemToArray(position, element)) { goto error; }
  }

  cJSON* normal = cJSON_AddArrayToObject(hittable, "normal");
  if (!normal) { goto error; }

  for (usize i = 0; i < 3; i++) {
    cJSON* element = cJSON_CreateNumber(plane->normal.data[i]);
    if (!element) { goto error; }

    if (!cJSON_AddItemToArray(normal, element)) { goto error; }
  }

  cJSON* size = cJSON_AddArrayToObject(hittable, "size");
  if (!size) { goto error; }

  for (usize i = 0; i < 2; i++) {
    cJSON* element = cJSON_CreateNumber(plane->size.data[i]);
    if (!element) { goto error; }

    if (!cJSON_AddItemToArray(size, element)) { goto error; }
  }

  return hittable;

error:
  fprintf(stderr, "[ERROR] [HITTABLE] [PLANE] Failed to create JSON object!\n");
  cJSON_Delete(hittable);
  return NULL;
}

HittablePlane* hittable_plane_json_parse(cJSON* plane_json) {
  cJSON* position = cJSON_GetObjectItemCaseSensitive(plane_json, "position");
  if (!position || !cJSON_IsArray(position)) { goto error; }

  cJSON* normal = cJSON_GetObjectItemCaseSensitive(plane_json, "normal");
  if (!normal || !cJSON_IsArray(normal)) { goto error; }

  cJSON* size = cJSON_GetObjectItemCaseSensitive(plane_json, "size");
  if (!size || !cJSON_IsArray(size)) { goto error; }

  Vector3 new_position = { cJSON_GetNumberValue(cJSON_GetArrayItem(position, 0)), cJSON_GetNumberValue(cJSON_GetArrayItem(position, 1)), cJSON_GetNumberValue(cJSON_GetArrayItem(position, 2)) };
  Vector3 new_normal = { cJSON_GetNumberValue(cJSON_GetArrayItem(normal, 0)), cJSON_GetNumberValue(cJSON_GetArrayItem(normal, 1)), cJSON_GetNumberValue(cJSON_GetArrayItem(normal, 2)) };
  Vector2 new_size = { cJSON_GetNumberValue(cJSON_GetArrayItem(size, 0)), cJSON_GetNumberValue(cJSON_GetArrayItem(size, 1)) };
  return hittable_plane_create(new_position, new_normal, new_size, NULL);

error:
  fprintf(stderr, "[ERROR] [HITTABLE] [PLANE] Failed to parse JSON object!\n");
  return NULL;
}

void hittable_plane_destroy(HittablePlane* plane) {
  free(plane->hittable.material);
  free(plane);
}
