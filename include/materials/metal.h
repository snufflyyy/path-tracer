#pragma once

#include "materials/material.h"
#include "types/color.h"
#include "types/rayhit.h"

typedef struct Metal {
  Material material;

  Color albedo;
  f32 roughness;
} Metal;

Metal* metal_material_create(Color albedo, f32 roughness);
MaterialGetColorResult metal_material_get_color(Metal* metal, RayHit rayhit, u64* state);
void metal_material_destroy(Metal* metal);
