#include "materials/glass.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "materials/material.h"
#include "math/vector3.h"
#include "random.h"
#include "types/color.h"

static MaterialGetColorResult get_color(Material* material, RayHit rayhit, u64* state);
static void destroy(Material* material);

Glass* glass_material_create(Color albedo, f32 refraction_index, f32 roughness) {
  Glass* glass = (Glass*) malloc(sizeof(Glass));
  if (!glass) {
    fprintf(stderr, "[ERROR] [MATERIAL] [GLASS] Failed to allocate memory for glass material!\n");
    return NULL;
  }

  glass->material = (Material) { GLASS, get_color, destroy };
  glass->albedo = albedo;
  glass->refraction_index = refraction_index;
  glass->roughness = roughness;

  return glass;
}

inline static MaterialGetColorResult get_color(Material* material, RayHit rayhit, u64* state) {
  return glass_material_get_color((Glass*) material, rayhit, state);
}

inline static void destroy(Material* material) {
  glass_material_destroy((Glass*) material);
}

static f32 reflectance(f32 cosine, f32 refraction_ratio);

MaterialGetColorResult glass_material_get_color(Glass* glass, RayHit rayhit, u64* state) {
  MaterialGetColorResult result;

  result.color = glass->albedo;

  f32 refraction_ratio = rayhit.inside ? (1.0f / glass->refraction_index) : glass->refraction_index;
  f32 cos_theta = fmin(vector3_dot_product(vector3_scale(vector3_normalize(rayhit.ray.direction), -1.0f), rayhit.normal), 1.0f);
  f32 sin_theta = sqrtf(1.0f - (cos_theta * cos_theta));

  bool cannot_refract = refraction_ratio * sin_theta > 1.0f;
  bool should_reflect = reflectance(cos_theta, refraction_ratio) > random_f32(state); 

  if (cannot_refract || should_reflect) {
    result.direction = vector3_reflect(rayhit.ray.direction, rayhit.normal);
  } else {
    result.direction = vector3_refract(rayhit.ray.direction, rayhit.normal, refraction_ratio);
  }

  result.direction = vector3_add(result.direction, vector3_scale(vector3_random_unit_vector(state), glass->roughness));
  result.direction = vector3_normalize(result.direction);

  return result;
}

static f32 reflectance(f32 cosine, f32 refraction_ratio) {
  f32 reflectance = (1.0f - refraction_ratio) / (1.0f + refraction_ratio);
  reflectance *= reflectance;
  return reflectance + (1.0f - reflectance) * powf(1.0f - cosine, 5.0f);
}

void glass_material_destroy(Glass* glass) {
  free(glass);
}
