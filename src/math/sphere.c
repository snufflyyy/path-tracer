#include <math.h>
#include <stdbool.h>

#include "math/sphere.h"
#include "math/ray.h"
#include "math/vector3.h"
#include "rayhit.h"

RayHit sphere_ray_hit(Sphere* sphere, Ray* ray) {
  Vector3 oc = vector3_subtract(sphere->position, ray->origin);
  f32 a = vector3_length_squared(ray->direction);
  f32 h = vector3_dot_product(ray->direction, oc);
  f32 c = vector3_length_squared(oc) - (sphere->radius * sphere->radius);
  f32 discriminant = (h * h) - (a * c);

  if (discriminant < 0.001f) {
    return (RayHit) {0};
  }

  f32 t = (h - sqrtf(discriminant)) / a;
  Vector3 hit_position = ray_at(ray, t);

  RayHit rayhit = {
    .hit = true,
    .t = t,
    .hit_position = hit_position,
    .normal = vector3_normalized(vector3_subtract(hit_position, sphere->position)),
    .color = (Color) { 0.75f, 0.75f, 0.75f }
  };

  return rayhit;
}
