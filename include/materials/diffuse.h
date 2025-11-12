#pragma once

#include <cJSON.h>

#include "textures/texture.h"
#include "materials/material.h"
#include "types/color.h"
#include "types/rayhit.h"

typedef struct MaterialDiffuse {
  Material material;

  Texture* albedo;
} MaterialDiffuse;

MaterialDiffuse* material_diffuse_create(Texture* texture);
Color material_diffuse_get_color(MaterialDiffuse* diffuse, Vector2 uv_coordinates);
Vector3 material_diffuse_get_direction(MaterialDiffuse* diffuse, RayHit rayhit, u64* state);

cJSON* material_diffuse_json_create(MaterialDiffuse* diffuse);
MaterialDiffuse* material_diffuse_json_parse(cJSON* diffuse_json);

void material_diffuse_destroy(MaterialDiffuse* diffuse);
