#pragma once

#include <cJSON.h>

#include "materials/material.h"
#include "textures/texture.h"
#include "types/color.h"
#include "types/rayhit.h"

typedef struct Metal {
  Material material;

  Texture* albedo;
  f32 roughness;
} Metal;

Metal* material_metal_create(Texture* albedo, f32 roughness);
Color material_metal_get_color(Metal* metal, Vector2 uv_coordinates);
Vector3 material_metal_get_direction(Metal* metal, RayHit rayhit, u64* state);

cJSON* material_metal_json_create(Metal* metal);
Metal* material_metal_json_parse(cJSON* metal_json);

void material_metal_destroy(Metal* metal);
