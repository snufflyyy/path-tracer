#pragma once

#include "materials/material.h"
#include "types/color.h"
#include "types/rayhit.h"

typedef struct Emissive {
  Material material;

  Color albedo;
  f32 emission_strength;
} Emissive;

Emissive* emissive_material_create(Color albedo, f32 emission_strength);
MaterialGetColorResult emissive_material_get_color(Emissive* diffuse, RayHit rayhit, u64* state);
void emissive_material_destroy(Emissive* diffuse);
