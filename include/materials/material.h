#pragma once

#include <stdbool.h>

#include "types/color.h"
#include "math/vector3.h"

typedef enum MaterialType {
  DIFFUSE,
  METAL,
  GLASS,
  EMISSIVE
} MaterialType;

struct RayHit;
typedef struct Material {
  MaterialType type;

  Color (*get_color)(struct Material* material);
  Vector3 (*get_direction)(struct Material* material, struct RayHit rayhit, u64* state); // the return vector will be normalized
  void (*destroy)(struct Material* material);
} Material;
