#pragma once

#include <cJSON.h>

#include "materials/material.h"
#include "types/color.h"
#include "textures/texture.h"
#include "types/rayhit.h"

typedef struct MaterialEmissive {
  Material material;

  Texture* albedo;
  f32 emission_strength;
} MaterialEmissive;

MaterialEmissive* material_emissive_create(Texture* albedo, f32 emission_strength);
Color material_emissive_get_color(MaterialEmissive* emissive, Vector2 uv_coordinates);
Vector3 material_emissive_get_direction(MaterialEmissive* diffuse, RayHit rayhit, u64* state);

cJSON* material_emissive_json_create(MaterialEmissive* emissive);
MaterialEmissive* material_emissive_json_parse(cJSON* emissive_json);

void material_emissive_destroy(MaterialEmissive* diffuse);
