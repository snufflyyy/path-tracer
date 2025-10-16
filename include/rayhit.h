#pragma once

#include <stdbool.h>

#include "types/material.h"
#include "math/vector3.h"

typedef struct RayHit {
  bool hit;

  f32 t;
  Vector3 hit_position;
  Vector3 normal;

  Material material;
} RayHit;
