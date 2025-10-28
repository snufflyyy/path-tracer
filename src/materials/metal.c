#include "materials/metal.h"

#include <stdlib.h>
#include <stdio.h>

#include "materials/material.h"
#include "math/vector3.h"
#include "random.h"
#include "types/color.h"

static Color get_color(Material* material);
static Vector3 get_direction(Material* material, RayHit rayhit, u64* state);
static void destroy(Material* material);

Metal* material_metal_create(Color albedo, f32 roughness) {
  Metal* metal = (Metal*) malloc(sizeof(Metal));
  if (!metal) {
    fprintf(stderr, "[ERROR] [MATERIAL] [METAL] Failed to allocate memory for metal material!\n");
    return NULL;
  }

  metal->material = (Material) { METAL, get_color, get_direction, destroy };
  metal->albedo = albedo;
  metal->roughness = roughness;

  return metal;
}

inline static Color get_color(Material* material) {
  return material_metal_get_color((Metal*) material);
}

inline static Vector3 get_direction(Material* material, RayHit rayhit, u64* state) {
  return material_metal_get_direction((Metal*) material, rayhit, state);
}

inline static void destroy(Material* material) {
  material_metal_destroy((Metal*) material);
}

inline Color material_metal_get_color(Metal* metal) {
  return metal->albedo;
}

inline Vector3 material_metal_get_direction(Metal* metal, RayHit rayhit, u64 *state) {
  return vector3_add(vector3_reflect(rayhit.ray.direction, rayhit.normal), vector3_scale(random_vector3_unit_vector(state), metal->roughness));
}

inline void material_metal_destroy(Metal* metal) {
  free(metal);
}
