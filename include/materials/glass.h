#pragma once

#include <cJSON.h>

#include "materials/material.h"
#include "types/color.h"
#include "types/rayhit.h"
#include "textures/texture.h"

typedef struct MaterialGlass {
  Material material;

  Texture* albedo;
  f32 refraction_index;
  f32 roughness;
} MaterialGlass;

MaterialGlass* material_glass_create(Texture* albedo, f32 refraction_index, f32 roughness);
Color material_glass_get_color(MaterialGlass* glass, Vector2 uv_coordinates);
Vector3 material_glass_get_direction(MaterialGlass* glass, RayHit rayhit, u64* state);

cJSON* material_glass_json_create(MaterialGlass* glass);
MaterialGlass* material_glass_json_parse(cJSON* glass_json);

void material_glass_destroy(MaterialGlass* glass);
