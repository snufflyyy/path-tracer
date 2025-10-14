#include "math/ray.h"
#include "math/vector3.h"

Vector3 ray_at(Ray* ray, f32 t) {
  return vector3_add(ray->origin, vector3_scale(ray->direction, t));
}
