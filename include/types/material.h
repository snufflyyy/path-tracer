#pragma once

#include <stdbool.h>

#include "types/color.h"
#include "types/base_types.h"

#define MATERIAL_GRAY (Material) { (Color) { 0.75f, 0.75f, 0.75f }, 1.0f, 0.0f, false, 1.33 }
#define MATERIAL_RED (Material) { (Color) { 1.0f, 0.2f, 0.2f }, 1.0f, 0.0, false, 1.33 }

typedef struct Material {
  Color albedo;
  f32 roughness;
  f32 emission_strength;

  bool glass;
  f32 refraction_index;
} Material;
