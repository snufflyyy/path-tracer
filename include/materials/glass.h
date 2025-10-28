#pragma once

#include "materials/material.h"
#include "types/color.h"
#include "types/rayhit.h"

typedef struct Glass {
  Material material;

  Color albedo;
  f32 refraction_index;
  f32 roughness;
} Glass;

Glass* material_glass_create(Color albedo, f32 refraction_index, f32 roughness);
Color material_glass_get_color(Glass* glass);
Vector3 material_glass_get_direction(Glass* glass, RayHit rayhit, u64* state);
void material_glass_destroy(Glass* glass);
