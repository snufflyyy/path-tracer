#include "materials/metal.h"

#include <stdlib.h>
#include <stdio.h>

#include "materials/material.h"
#include "math/vector3.h"
#include "random.h"
#include "textures/texture.h"
#include "textures/solid_color.h"
#include "types/color.h"

static Color get_color(Material* material, Vector2 uv_coordinates);
static Vector3 get_direction(Material* material, RayHit rayhit, u64* state);
static void destroy(Material* material);

Metal* material_metal_create(Texture* albedo, f32 roughness) {
  Metal* metal = (Metal*) malloc(sizeof(Metal));
  if (!metal) {
    fprintf(stderr, "[ERROR] [MATERIAL] [METAL] Failed to allocate memory for metal material!\n");
    return NULL;
  }

  metal->material = (Material) { MATERIAL_TYPE_METAL, get_color, get_direction, destroy };
  metal->albedo = albedo;
  metal->roughness = roughness;

  return metal;
}

inline static Color get_color(Material* material, Vector2 uv_coordinates) {
  return material_metal_get_color((Metal*) material, uv_coordinates);
}

inline static Vector3 get_direction(Material* material, RayHit rayhit, u64* state) {
  return material_metal_get_direction((Metal*) material, rayhit, state);
}

inline static void destroy(Material* material) {
  material_metal_destroy((Metal*) material);
}

inline Color material_metal_get_color(Metal* metal, Vector2 uv_coordinates) {
  return metal->albedo->get_color(metal->albedo, uv_coordinates);
}

inline Vector3 material_metal_get_direction(Metal* metal, RayHit rayhit, u64 *state) {
  return vector3_add(vector3_reflect(rayhit.ray.direction, rayhit.normal), vector3_scale(random_vector3_unit_vector(state), metal->roughness));
}

cJSON* material_metal_json_create(Metal* metal) {
  cJSON* metal_json = cJSON_CreateObject();
  if (!metal_json) { goto error; }

  if (!cJSON_AddNumberToObject(metal_json, "type", MATERIAL_TYPE_METAL)) { goto error; }

  cJSON* albedo_json = texture_json_create(metal->albedo);
  if (!albedo_json) { goto error; }

  if (!cJSON_AddItemToObject(metal_json, "albedo", albedo_json)) { goto error; }
  if (!cJSON_AddNumberToObject(metal_json, "roughness", metal->roughness)) { goto error; }

  return metal_json;

error:
  fprintf(stderr, "[ERROR] [MATERIAL] [METAL] Failed to create JSON object!\n");
  cJSON_Delete(metal_json);
  return NULL;
}

Metal* material_metal_json_parse(cJSON* metal_json) {
  cJSON* albedo_json = cJSON_GetObjectItemCaseSensitive(metal_json, "albedo");
  if (!albedo_json) { goto error; }

  Texture* texture = texture_json_parse(albedo_json);
  if (!texture) { goto error; }

  cJSON* roughness_json = cJSON_GetObjectItemCaseSensitive(metal_json, "roughness");
  if (!roughness_json) { goto error; }

  return material_metal_create(texture, cJSON_GetNumberValue(roughness_json));

error:
  fprintf(stderr, "[ERROR] [MATERIAL] [METAL] Failed to parse JSON object!\n");
  return NULL;
}

inline void material_metal_destroy(Metal* metal) {
  metal->albedo->destroy(metal->albedo);
  free(metal);
}
