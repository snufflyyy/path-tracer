#include "random.h"

#include <stdbool.h>
#include <math.h>

#include "types/base_types.h"
#include "math/vector3.h"
#include "hittables/hittable.h"
#include "hittables/sphere.h"
#include "hittables/plane.h"

u32 pcg32(u64* state) {
    u64 oldstate = *state;
    *state = oldstate * 6364136223846793005ULL + 1u;
    u32 xorshifted = (u32)(((oldstate >> 18u) ^ oldstate) >> 27u);
    u32 rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

inline f32 random_f32(u64* state) {
  return (pcg32(state) >> 8) * (1.0f / 16777216.0f);
}

inline f32 random_f32_range(u64* state, f32 min, f32 max) {
  return min + random_f32(state) * (max - min);
}

inline Vector3 random_vector3(u64* state, f32 min, f32 max) {
  return (Vector3) { random_f32_range(state, min, max), random_f32_range(state, min, max), random_f32_range(state, min, max) };
}

Vector3 random_vector3_unit_vector(u64* state) {
  while (true) {
    Vector3 p = random_vector3(state, -1.0f, 1.0f);
    f32 length_squared = vector3_length_squared(p);
    if (length_squared <= 1.0f) {
      return vector3_scale(p, 1.0f / sqrtf(length_squared));
    }
  }
}

Vector3 random_vector3_in_hemisphere(u64* state, Vector3 normal) {
  Vector3 random_unit_vector = random_vector3_unit_vector(state);
  if (vector3_dot_product(random_unit_vector, normal) < 0.0f) {
    return vector3_scale(random_unit_vector, -1.0f);
  }

  return random_unit_vector;
}

Vector3 random_hittable_position(u64* state, Hittable* hittable) {
  switch (hittable->type) {
    case HITTABLE_TYPE_SPHERE: {
      HittableSphere* sphere = (HittableSphere*) hittable;
      return vector3_add(sphere->position, vector3_scale(random_vector3_unit_vector(state), sphere->radius));
    } break;
    case HITTABLE_TYPE_PLANE: {
      HittablePlane* plane = (HittablePlane*) hittable;
      return vector3_add(plane->position, (Vector3) {  });
    } break;
  }

  return (Vector3) {0};
}
