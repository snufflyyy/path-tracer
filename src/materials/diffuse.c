#include "materials/diffuse.h"

#include <stdlib.h>
#include <stdio.h>

#include "types/color.h"
#include "materials/material.h"
#include "math/vector3.h"
#include "types/rayhit.h"
#include "types/base_types.h"
#include "random.h"

static Color get_color(Material* material);
static Vector3 get_direction(Material* material, RayHit rayhit, u64* state);
static void destroy(Material* material);

Diffuse* material_diffuse_create(Color albedo) {
  Diffuse* diffuse = (Diffuse*) malloc(sizeof(Diffuse));
  if (!diffuse) {
    fprintf(stderr, "[ERROR] [MATERIAL] [DIFFUSE] Failed to allocate memory for diffuse material!\n");
    return NULL;
  }

  diffuse->material = (Material) { DIFFUSE, get_color, get_direction, destroy };
  diffuse->albedo = albedo;

  return diffuse;
}

inline static Color get_color(Material* material) {
  return material_diffuse_get_color((Diffuse*) material);
}

inline static Vector3 get_direction(Material* material, RayHit rayhit, u64* state) {
  return material_diffuse_get_direction((Diffuse*) material, rayhit, state);
}

inline static void destroy(Material* material) {
  material_diffuse_destroy((Diffuse*) material);
}

inline Color material_diffuse_get_color(Diffuse* diffuse) {
  return diffuse->albedo;
}

inline Vector3 material_diffuse_get_direction(Diffuse *diffuse, RayHit rayhit, u64 *state) {
  return vector3_normalize(vector3_add(rayhit.normal, random_vector3_unit_vector(state)));
}

inline void material_diffuse_destroy(Diffuse* diffuse) {
  free(diffuse);
}
