#pragma once

#include "materials/material.h"
#include "types/color.h"
#include "types/rayhit.h"

typedef struct Diffuse {
  Material material;

  Color albedo;
} Diffuse;

Diffuse* material_diffuse_create(Color albedo);
Color material_diffuse_get_color(Diffuse* diffuse);
Vector3 material_diffuse_get_direction(Diffuse* diffuse, RayHit rayhit, u64* state);
void material_diffuse_destroy(Diffuse* diffuse);
