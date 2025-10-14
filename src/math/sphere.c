#include "math/sphere.h"

bool sphere_ray_hit(Sphere* sphere, Ray* ray) {
  Vector3 oc = vector3_subtract(sphere->position, ray->origin);
  float a = vector3_dot_product(ray->direction, ray->direction);
  float b = -2.0f * vector3_dot_product(ray->direction, oc);
  float c = vector3_dot_product(oc, oc) - (sphere->radius * sphere->radius);
  float discriminant = (b * b) - (4.0f * a * c);

  return (discriminant >= 0);
}
