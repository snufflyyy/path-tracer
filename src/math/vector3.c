#include "math/vector3.h"
#include "types/base_types.h"

#include <stdbool.h>
#include <math.h>

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

inline f32 vector3_length_squared(Vector3 a) {
  f32 length = vector3_length(a);
  return length * length;
}

inline Vector3 vector3_normalize(Vector3 a) {
  f32 length = vector3_length(a);
  return (Vector3) { a.x / length, a.y / length, a.z / length };
}

inline Vector3 vector3_reflect(Vector3 a, Vector3 normal) {
  return vector3_subtract(a, vector3_scale(normal, 2.0f * vector3_dot_product(a, normal)));
}

Vector3 vector3_refract(Vector3 a, Vector3 normal, f32 refraction_ratio) {
  f32 cos_theta = fmin(vector3_dot_product(vector3_scale(a, -1.0f), normal), 1.0f);
  Vector3 perpendicular = vector3_scale(vector3_add(a, vector3_scale(normal, cos_theta)), refraction_ratio);
  Vector3 parallel = vector3_scale(normal, -sqrtf(fabs(1.0f - vector3_length_squared(perpendicular))));
  return vector3_add(perpendicular, parallel);
}

inline f32 vector3_dot_product(Vector3 a, Vector3 b) {
  return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}
