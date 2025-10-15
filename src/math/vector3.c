#include "math/vector3.h"
#include "types/base_types.h"

#include <stdbool.h>
#include <math.h>

inline Vector3 vector3_random(f32 min, f32 max) {
  return (Vector3) { random_f32_range(min, max), random_f32_range(min, max), random_f32_range(min, max) };
}

Vector3 vector3_random_unit_vector() {
  while (true) {
    Vector3 p = vector3_random(-1.0f, 1.0f);
    f32 length_squared = vector3_length_squared(p);
    if (length_squared <= 1.0f) {
      return vector3_scale(p, 1.0f / sqrt(length_squared));
    }
  }
}

Vector3 vector3_random_in_hemisphere(Vector3 normal) {
  Vector3 random_unit_vector = vector3_random_unit_vector();
  if (vector3_dot_product(random_unit_vector, normal) < 0.0f) {
    return vector3_scale(random_unit_vector, -1.0f);
  }
  return random_unit_vector;
}

inline Vector3 vector3_add(Vector3 a, Vector3 b) {
  return (Vector3) { a.x + b.x, a.y + b.y, a.z + b.z };
}

inline Vector3 vector3_subtract(Vector3 a, Vector3 b) {
  return (Vector3) { a.x - b.x, a.y - b.y, a.z - b.z };
}

inline Vector3 vector3_scale(Vector3 a, f32 scalar) {
  return (Vector3) { a.x * scalar, a.y * scalar, a.z * scalar };
}

inline f32 vector3_length(Vector3 a) {
  return sqrt((a.x * a.x) + (a.y * a.y) + (a.z * a.z));
}

f32 vector3_length_squared(Vector3 a) {
  f32 length = vector3_length(a);
  return length * length;
}

inline Vector3 vector3_normalized(Vector3 a) {
  f32 length = vector3_length(a);
  return (Vector3) { a.x / length, a.y / length, a.z / length };
}

inline f32 vector3_dot_product(Vector3 a, Vector3 b) {
  return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}
