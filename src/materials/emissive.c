#include "materials/emissive.h"

#include <stdlib.h>
#include <stdio.h>

#include "materials/material.h"
#include "math/vector3.h"
#include "random.h"
#include "types/color.h"
#include "types/rayhit.h"

static Color get_color(Material* material);
static Vector3 get_direction(Material* material, RayHit rayhit, u64* state);
static void destroy(Material* material);

Emissive* material_emissive_create(Color albedo, f32 emission_strength) {
  Emissive* emissive = (Emissive*) malloc(sizeof(Emissive));
  if (!emissive) {
    fprintf(stderr, "[ERROR] [MATERIAL] [EMISSIVE] Failed to allocate memory for emissive material!\n");
    return NULL;
  }

  emissive->material = (Material) { EMISSIVE, get_color, get_direction, destroy };
  emissive->albedo = albedo;
  emissive->emission_strength = emission_strength;

  return emissive;
}

inline static Color get_color(Material* material) {
  return material_emissive_get_color((Emissive*) material);
}

inline static Vector3 get_direction(Material* material, RayHit rayhit, u64* state) {
  return material_emissive_get_direction((Emissive*) material, rayhit, state);
}

inline static void destroy(Material* material) {
  material_emissive_destroy((Emissive*) material);
}

inline Color material_emissive_get_color(Emissive* emissive) {
  return color_add(emissive->albedo, color_scale(emissive->albedo, emissive->emission_strength));
}

inline Vector3 material_emissive_get_direction(Emissive* emissive, RayHit rayhit, u64* state) {
  return vector3_normalize(vector3_add(rayhit.normal, random_vector3_unit_vector(state)));
}

inline void material_emissive_destroy(Emissive* emissive) {
  free(emissive);
}
