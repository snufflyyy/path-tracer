#include "materials/glass.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "types/color.h"
#include "materials/material.h"
#include "math/vector3.h"
#include "types/rayhit.h"
#include "types/base_types.h"
#include "random.h"

static Color get_color(Material* material);
static Vector3 get_direction(Material* material, RayHit rayhit, u64* state);
static void destroy(Material* material);

Glass* glass_material_create(Color albedo, f32 refraction_index, f32 roughness) {
  Glass* glass = (Glass*) malloc(sizeof(Glass));
  if (!glass) {
    fprintf(stderr, "[ERROR] [MATERIAL] [GLASS] Failed to allocate memory for glass material!\n");
    return NULL;
  }

  glass->material = (Material) { GLASS, get_color, get_direction, destroy };
  glass->albedo = albedo;
  glass->refraction_index = refraction_index;
  glass->roughness = roughness;

  return glass;
}

inline static Color get_color(Material* material) {
  return glass_material_get_color((Glass*) material);
}

inline static Vector3 get_direction(Material* material, RayHit rayhit, u64* state) {
  return glass_material_get_direction((Glass*) material, rayhit, state);
}

inline static void destroy(Material* material) {
  glass_material_destroy((Glass*) material);
}

inline Color glass_material_get_color(Glass* glass) {
  return glass->albedo;
}

static f32 reflectance(f32 cosine, f32 refraction_ratio);

Vector3 glass_material_get_direction(Glass *glass, RayHit rayhit, u64 *state) {
  Vector3 direction;

  f32 refraction_ratio = rayhit.inside ? (1.0f / glass->refraction_index) : glass->refraction_index;
  f32 cos_theta = fmin(vector3_dot_product(vector3_scale(vector3_normalize(rayhit.ray.direction), -1.0f), rayhit.normal), 1.0f);
  f32 sin_theta = sqrtf(1.0f - (cos_theta * cos_theta));

  bool cannot_refract = refraction_ratio * sin_theta > 1.0f;
  bool should_reflect = reflectance(cos_theta, refraction_ratio) > random_f32(state); 

  if (cannot_refract || should_reflect) {
    direction = vector3_reflect(rayhit.ray.direction, rayhit.normal);
  } else {
    direction = vector3_refract(rayhit.ray.direction, rayhit.normal, refraction_ratio);
  }

  direction = vector3_normalize(vector3_add(direction, vector3_scale(random_vector3_unit_vector(state), glass->roughness)));

  return direction;
}

static f32 reflectance(f32 cosine, f32 refraction_ratio) {
  f32 reflectance = (1.0f - refraction_ratio) / (1.0f + refraction_ratio);
  reflectance *= reflectance;
  return reflectance + (1.0f - reflectance) * powf(1.0f - cosine, 5.0f);
}

inline void glass_material_destroy(Glass* glass) {
  free(glass);
}
