#include "materials/diffuse.h"

#include <stdlib.h>
#include <stdio.h>

#include "cJSON.h"
#include "math/vector2.h"
#include "types/color.h"
#include "materials/material.h"
#include "math/vector3.h"
#include "types/rayhit.h"
#include "types/base_types.h"
#include "random.h"
#include "textures/texture.h"
#include "textures/solid_color.h"

static Color get_color(Material* material, Vector2 uv_coordinates);
static Vector3 get_direction(Material* material, RayHit rayhit, u64* state);
static void destroy(Material* material);

MaterialDiffuse* material_diffuse_create(Texture* albedo) {
  MaterialDiffuse* diffuse = (MaterialDiffuse*) malloc(sizeof(MaterialDiffuse));
  if (!diffuse) {
    fprintf(stderr, "[ERROR] [MATERIAL] [DIFFUSE] Failed to allocate memory for diffuse material!\n");
    return NULL;
  }

  diffuse->material = (Material) { MATERIAL_TYPE_DIFFUSE, get_color, get_direction, destroy };
  diffuse->albedo = albedo;

  return diffuse;
}

inline static Color get_color(Material* material, Vector2 uv_coordinates) {
  return material_diffuse_get_color((MaterialDiffuse*) material, uv_coordinates);
}

inline static Vector3 get_direction(Material* material, RayHit rayhit, u64* state) {
  return material_diffuse_get_direction((MaterialDiffuse*) material, rayhit, state);
}

inline static void destroy(Material* material) {
  material_diffuse_destroy((MaterialDiffuse*) material);
}

inline Color material_diffuse_get_color(MaterialDiffuse* diffuse, Vector2 uv_coordinates) {
  return diffuse->albedo->get_color(diffuse->albedo, uv_coordinates);
}

inline Vector3 material_diffuse_get_direction(MaterialDiffuse *diffuse, RayHit rayhit, u64 *state) {
  return vector3_normalize(vector3_add(rayhit.normal, random_vector3_unit_vector(state)));
}

cJSON* material_diffuse_json_create(MaterialDiffuse* diffuse) {
  cJSON* diffuse_json = cJSON_CreateObject();
  if (!diffuse_json) { goto error; }

  if (!cJSON_AddNumberToObject(diffuse_json, "type", MATERIAL_TYPE_DIFFUSE)) { goto error; }

  cJSON* albedo_json = texture_json_create(diffuse->albedo);
  if (!albedo_json) { goto error;}

  if (!cJSON_AddItemToObject(diffuse_json, "albedo", albedo_json)) { goto error; }

  return diffuse_json;

error:
  fprintf(stderr, "[ERROR] [MATERIAL] [DIFFUSE] Failed to create JSON object!\n");
  cJSON_Delete(diffuse_json);
  return NULL;
}

MaterialDiffuse* material_diffuse_json_parse(cJSON* diffuse_json) {
  cJSON* albedo_json = cJSON_GetObjectItemCaseSensitive(diffuse_json, "albedo");
  if (!albedo_json || !cJSON_IsObject(albedo_json)) { goto error; }

  Texture* texture = texture_json_parse(albedo_json);
  if (!texture) { goto error; }

  return material_diffuse_create(texture);

error:
  fprintf(stderr, "[ERROR] [MATERIAL] [DIFFUSE] Failed to parse JSON object!\n");
  return NULL;
}

inline void material_diffuse_destroy(MaterialDiffuse* diffuse) {
  diffuse->albedo->destroy(diffuse->albedo);
  free(diffuse);
}
