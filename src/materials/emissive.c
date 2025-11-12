#include "materials/emissive.h"

#include <stdlib.h>
#include <stdio.h>

#include "materials/material.h"
#include "math/vector2.h"
#include "math/vector3.h"
#include "random.h"
#include "textures/texture.h"
#include "types/color.h"
#include "types/rayhit.h"
#include "textures/solid_color.h"

static Color get_color(Material* material, Vector2 uv_coordinates);
static Vector3 get_direction(Material* material, RayHit rayhit, u64* state);
static void destroy(Material* material);

MaterialEmissive* material_emissive_create(Texture* albedo, f32 emission_strength) {
  MaterialEmissive* emissive = (MaterialEmissive*) malloc(sizeof(MaterialEmissive));
  if (!emissive) {
    fprintf(stderr, "[ERROR] [MATERIAL] [EMISSIVE] Failed to allocate memory for emissive material!\n");
    return NULL;
  }

  emissive->material = (Material) { MATERIAL_TYPE_EMISSIVE, get_color, get_direction, destroy };
  emissive->albedo = albedo;
  emissive->emission_strength = emission_strength;

  return emissive;
}

inline static Color get_color(Material* material, Vector2 uv_coordinates) {
  return material_emissive_get_color((MaterialEmissive*) material, uv_coordinates);
}

inline static Vector3 get_direction(Material* material, RayHit rayhit, u64* state) {
  return material_emissive_get_direction((MaterialEmissive*) material, rayhit, state);
}

inline static void destroy(Material* material) {
  material_emissive_destroy((MaterialEmissive*) material);
}

inline Color material_emissive_get_color(MaterialEmissive* emissive, Vector2 uv_coordinates) {
  Color emissive_color = emissive->albedo->get_color(emissive->albedo, uv_coordinates);
  return color_add(emissive_color, color_scale(emissive_color, emissive->emission_strength));
}

inline Vector3 material_emissive_get_direction(MaterialEmissive* emissive, RayHit rayhit, u64* state) {
  return vector3_normalize(vector3_add(rayhit.normal, random_vector3_unit_vector(state)));
}

cJSON* material_emissive_json_create(MaterialEmissive* emissive) {
  cJSON* emissive_json = cJSON_CreateObject();
  if (!emissive_json) { goto error; }

  if (!cJSON_AddNumberToObject(emissive_json, "type", MATERIAL_TYPE_EMISSIVE)) { goto error; }

  cJSON* albedo_json = texture_json_create(emissive->albedo);
  if (!albedo_json) { goto error;}

  if (!cJSON_AddItemToObject(emissive_json, "albedo", albedo_json)) { goto error; }
  if (!cJSON_AddNumberToObject(emissive_json, "emission-strength", emissive->emission_strength)) { goto error; }

  return emissive_json;

error:
  fprintf(stderr, "[ERROR] [MATERIAL] [EMISSIVE] Failed to create JSON object!\n");
  cJSON_Delete(emissive_json);
  return NULL;
}

MaterialEmissive* material_emissive_json_parse(cJSON* emissive_json) {
  cJSON* albedo_json = cJSON_GetObjectItemCaseSensitive(emissive_json, "albedo");
  if (!albedo_json || !cJSON_IsObject(albedo_json)) { goto error; }

  Texture* texture = texture_json_parse(albedo_json);
  if (!texture) { goto error; }

  cJSON* emission_strength = cJSON_GetObjectItemCaseSensitive(emissive_json, "emission-strength");
  if (!emission_strength) { goto error; }

  return material_emissive_create(texture, cJSON_GetNumberValue(emission_strength));

error:
  fprintf(stderr, "[ERROR] [MATERIAL] [EMISSIVE] Failed to parse JSON object!\n");
  return NULL;
}

inline void material_emissive_destroy(MaterialEmissive* emissive) {
  emissive->albedo->destroy(emissive->albedo);
  free(emissive);
}
