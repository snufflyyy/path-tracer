#include "materials/glass.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "textures/texture.h"
#include "types/color.h"
#include "materials/material.h"
#include "math/vector3.h"
#include "types/rayhit.h"
#include "types/base_types.h"
#include "textures/solid_color.h"
#include "random.h"

static Color get_color(Material* material, Vector2 uv_coordinates);
static Vector3 get_direction(Material* material, RayHit rayhit, u64* state);
static void destroy(Material* material);

static f32 reflectance(f32 cosine, f32 refraction_ratio);

MaterialGlass* material_glass_create(Texture* albedo, f32 refraction_index, f32 roughness) {
  MaterialGlass* glass = (MaterialGlass*) malloc(sizeof(MaterialGlass));
  if (!glass) {
    fprintf(stderr, "[ERROR] [MATERIAL] [GLASS] Failed to allocate memory for glass material!\n");
    return NULL;
  }

  glass->material = (Material) { MATERIAL_TYPE_GLASS, get_color, get_direction, destroy };
  glass->albedo = albedo;
  glass->refraction_index = refraction_index;
  glass->roughness = roughness;

  return glass;
}

inline static Color get_color(Material* material, Vector2 uv_coordinates) {
  return material_glass_get_color((MaterialGlass*) material, uv_coordinates);
}

inline static Vector3 get_direction(Material* material, RayHit rayhit, u64* state) {
  return material_glass_get_direction((MaterialGlass*) material, rayhit, state);
}

inline static void destroy(Material* material) {
  material_glass_destroy((MaterialGlass*) material);
}

inline Color material_glass_get_color(MaterialGlass* glass, Vector2 uv_coordinates) {
  return glass->albedo->get_color(glass->albedo, uv_coordinates);
}

Vector3 material_glass_get_direction(MaterialGlass *glass, RayHit rayhit, u64 *state) {
  Vector3 direction;

  f32 refraction_ratio = rayhit.inside ? (1.0f / glass->refraction_index) : glass->refraction_index;
  f32 cos_theta = fmin(vector3_dot_product(vector3_scale(vector3_normalize(rayhit.ray.direction), -1.0f), rayhit.normal), 1.0f);
  f32 sin_theta = sqrtf(1.0f - (cos_theta * cos_theta));

  bool cannot_refract = refraction_ratio * sin_theta > 1.0f;
  bool should_reflect = reflectance(cos_theta, refraction_ratio) > random_f32(state);

  if (cannot_refract || should_reflect) {
    direction = vector3_reflect(rayhit.ray.direction, rayhit.normal);
  } else {
    direction = vector3_refract(rayhit.ray.direction, rayhit.normal, refraction_ratio);
  }

  direction = vector3_normalize(vector3_add(direction, vector3_scale(random_vector3_unit_vector(state), glass->roughness)));

  return direction;
}

static f32 reflectance(f32 cosine, f32 refraction_ratio) {
  f32 reflectance = (1.0f - refraction_ratio) / (1.0f + refraction_ratio);
  reflectance *= reflectance;
  return reflectance + (1.0f - reflectance) * powf(1.0f - cosine, 5.0f);
}

cJSON* material_glass_json_create(MaterialGlass* glass) {
  cJSON* glass_json = cJSON_CreateObject();
  if (!glass_json) { goto error; }

  if (!cJSON_AddNumberToObject(glass_json, "type", MATERIAL_TYPE_GLASS)) { goto error; }

  cJSON* albedo_json = texture_json_create(glass->albedo);
  if (!albedo_json) { goto error; }

  if (!cJSON_AddItemToObject(glass_json, "albedo", albedo_json)) { goto error; }
  if (!cJSON_AddNumberToObject(glass_json, "roughness", glass->roughness)) { goto error; }
  if (!cJSON_AddNumberToObject(glass_json, "refraction-index", glass->refraction_index)) { goto error; }

  return glass_json;

error:
  fprintf(stderr, "[ERROR] [MATERIAL] [GLASS] Failed to create JSON object!\n");
  cJSON_Delete(glass_json);
  return NULL;
}

MaterialGlass* material_glass_json_parse(cJSON* glass_json) {
  cJSON* albedo_json = cJSON_GetObjectItemCaseSensitive(glass_json, "albedo");
  if (!albedo_json) { goto error; }

  Texture* texture = texture_json_parse(albedo_json);
  if (!texture) { goto error; }

  cJSON* roughness = cJSON_GetObjectItemCaseSensitive(glass_json, "roughness");
  if (!roughness) { goto error; }

  cJSON* refraction_index = cJSON_GetObjectItemCaseSensitive(glass_json, "refraction-index");
  if (!roughness) { goto error; }

  return material_glass_create(texture, cJSON_GetNumberValue(refraction_index), cJSON_GetNumberValue(roughness));

error:
  fprintf(stderr, "[ERROR] [MATERIAL] [GLASS] Failed to parse JSON object!\n");
  return NULL;
}

inline void material_glass_destroy(MaterialGlass* glass) {
  free(glass);
}
