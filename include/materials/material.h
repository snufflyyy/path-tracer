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

typedef struct MaterialGetColorResult {
  Color color;
  Vector3 direction;
} MaterialGetColorResult;

struct RayHit;
typedef struct Material {
  MaterialType type;
  MaterialGetColorResult (*get_color)(struct Material* material, struct RayHit rayhit, u64* state);
  void (*destroy)(struct Material* material);
} Material;
