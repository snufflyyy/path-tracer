#include "materials/metal.h"
#include "materials/material.h"
#include "math/vector3.h"
#include "random.h"
#include "types/color.h"

#include <stdlib.h>
#include <stdio.h>

static MaterialGetColorResult get_color(Material* material, RayHit rayhit, u64* state);
static void destroy(Material* material);

Metal* metal_material_create(Color albedo, f32 roughness) {
  Metal* metal = (Metal*) malloc(sizeof(Metal));
  if (!metal) {
    fprintf(stderr, "[ERROR] [MATERIAL] [METAL] Failed to allocate memory for metal material!\n");
    return NULL;
  }

  metal->material = (Material) { METAL, get_color, destroy };
  metal->albedo = albedo;
  metal->roughness = roughness;

  return metal;
}

inline static MaterialGetColorResult get_color(Material* material, RayHit rayhit, u64* state) {
  return metal_material_get_color((Metal*) material, rayhit, state);
}

inline static void destroy(Material* material) {
  metal_material_destroy((Metal*) material);
}

MaterialGetColorResult metal_material_get_color(Metal* metal, RayHit rayhit, u64* state) {
  MaterialGetColorResult result;

  Vector3 reflected = vector3_reflect(rayhit.ray.direction, rayhit.normal);

  result.color = metal->albedo;
  result.direction = vector3_add(reflected, vector3_scale(vector3_random_unit_vector(state), metal->roughness));

  return result;
}

void metal_material_destroy(Metal* metal) {
  free(metal);
}
