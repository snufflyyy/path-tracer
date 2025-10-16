#pragma once

#include "types/color.h"
#include "types/base_types.h"

#define MATERIAL_GRAY (Material) { (Color) { 0.75f, 0.75f, 0.75f }, 1.0f, 0.0f }
#define MATERIAL_RED (Material) { (Color) { 1.0f, 0.2f, 0.2f }, 1.0f, 0.0 }

typedef struct Material {
  Color albedo;
  f32 roughness;
  f32 emission_strength;
} Material;
