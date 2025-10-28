#pragma once

#include "materials/material.h"
#include "types/color.h"
#include "types/rayhit.h"

typedef struct Metal {
  Material material;

  Color albedo;
  f32 roughness;
} Metal;

Metal* material_metal_create(Color albedo, f32 roughness);
Color material_metal_get_color(Metal* metal);
Vector3 material_metal_get_direction(Metal* metal, RayHit rayhit, u64* state);
void material_metal_destroy(Metal* metal);
