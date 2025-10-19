#include "materials/emission.h"

#include <stdlib.h>
#include <stdio.h>

#include "materials/material.h"
#include "random.h"
#include "types/color.h"
#include "types/rayhit.h"

static MaterialGetColorResult get_color(Material* material, RayHit rayhit, u64* state);
static void destroy(Material* material);

Emissive* emissive_material_create(Color albedo, f32 emission_strength) {
  Emissive* emissive = (Emissive*) malloc(sizeof(Emissive));
  if (!emissive) {
    fprintf(stderr, "[ERROR] [MATERIAL] [DIFFUSE] Failed to allocate memory for diffuse material!\n");
    return NULL;
  }

  emissive->material = (Material) { EMISSIVE, get_color, destroy };
  emissive->albedo = albedo;
  emissive->emission_strength = emission_strength;

  return emissive;
}

inline static MaterialGetColorResult get_color(Material* material, RayHit rayhit, u64* state) {
  return emissive_material_get_color((Emissive*) material, rayhit, state);
}
inline static void destroy(Material* material) {
  emissive_material_destroy((Emissive*) material);
}

MaterialGetColorResult emissive_material_get_color(Emissive* emissive, RayHit rayhit, u64* state) {
  MaterialGetColorResult result;

  result.color = color_add(emissive->albedo, color_scale(emissive->albedo, emissive->emission_strength));
  result.direction = vector3_add(rayhit.normal, vector3_random_unit_vector(state));

  return result;
}

void emissive_material_destroy(Emissive* emissive) {
  free(emissive);
}
