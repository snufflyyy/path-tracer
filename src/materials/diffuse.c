#include "materials/diffuse.h"

#include <stdlib.h>
#include <stdio.h>

#include "materials/material.h"
#include "random.h"
#include "types/color.h"
#include "types/rayhit.h"

static MaterialGetColorResult get_color(Material* material, RayHit rayhit, u64* state);
static void destroy(Material* material);

Diffuse* diffuse_material_create(Color albedo) {
  Diffuse* diffuse = (Diffuse*) malloc(sizeof(Diffuse));
  if (!diffuse) {
    fprintf(stderr, "[ERROR] [MATERIAL] [DIFFUSE] Failed to allocate memory for diffuse material!\n");
    return NULL;
  }

  diffuse->material = (Material) { DIFFUSE, get_color, destroy };
  diffuse->albedo = albedo;

  return diffuse;
}

inline static MaterialGetColorResult get_color(Material* material, RayHit rayhit, u64* state) {
  return diffuse_material_get_color((Diffuse*) material, rayhit, state);
}
inline static void destroy(Material* material) {
  diffuse_material_destroy((Diffuse*) material);
}

MaterialGetColorResult diffuse_material_get_color(Diffuse* diffuse, RayHit rayhit, u64* state) {
  MaterialGetColorResult result;

  result.color = diffuse->albedo;
  result.direction = vector3_add(rayhit.normal, vector3_random_unit_vector(state));

  return result;
}

void diffuse_material_destroy(Diffuse* diffuse) {
  free(diffuse);
}
