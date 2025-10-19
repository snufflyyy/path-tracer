#pragma once

#include "materials/material.h"
#include "types/color.h"
#include "types/rayhit.h"

typedef struct Diffuse {
  Material material;

  Color albedo; 
} Diffuse;

Diffuse* diffuse_material_create(Color albedo);
MaterialGetColorResult diffuse_material_get_color(Diffuse* diffuse, RayHit rayhit, u64* state);
void diffuse_material_destroy(Diffuse* diffuse);
