#include "math/vector3.h"

inline Vector3 vector3_add(Vector3 a, Vector3 b) {
  return (Vector3) { a.x + b.x, a.y + b.y, a.z + b.z };
}

inline Vector3 vector3_subtract(Vector3 a, Vector3 b) {
  return (Vector3) { a.x - b.x, a.y - b.y, a.z - b.z };
}

inline Vector3 vector3_scale(Vector3 a, f32 scalar) {
  return (Vector3) { a.x * scalar, a.y * scalar, a.z * scalar };
}

inline f32 vector3_dot_product(Vector3 a, Vector3 b) {
  return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}
