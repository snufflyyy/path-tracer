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

Glass* glass_material_create(Color albedo, f32 refraction_index, f32 roughness);
Color glass_material_get_color(Glass* glass);
Vector3 glass_material_get_direction(Glass* glass, RayHit rayhit, u64* state);
void glass_material_destroy(Glass* glass);
