#pragma once

#include "materials/material.h"
#include "types/color.h"
#include "types/rayhit.h"

typedef struct Emissive {
  Material material;

  Color albedo;
  f32 emission_strength;
} Emissive;

Emissive* material_emissive_create(Color albedo, f32 emission_strength);
Color material_emissive_get_color(Emissive* diffuse);
Vector3 material_emissive_get_direction(Emissive* diffuse, RayHit rayhit, u64* state);
void material_emissive_destroy(Emissive* diffuse);
