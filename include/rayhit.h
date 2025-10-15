#pragma once

#include <stdbool.h>

#include "math/vector3.h"
#include "types/color.h"

typedef struct RayHit {
  bool hit;
  f32 t;
  Vector3 hit_position;
  Vector3 normal;
  Color color;
} RayHit;
