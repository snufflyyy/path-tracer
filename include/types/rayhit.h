#pragma once

#include <stdbool.h>

#include <math/ray.h>
#include "math/vector2.h"
#include "math/vector3.h"
#include "materials/material.h"

typedef struct RayHit {
  bool hit;

  Ray ray;
  f32 t;
  Vector2 uv_coordinates;
  Vector3 hit_position;
  Vector3 normal;
  bool inside;

  Material* material;
} RayHit;
