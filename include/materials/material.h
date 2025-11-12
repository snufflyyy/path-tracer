#pragma once

#include <stdbool.h>

#include "types/color.h"
#include "math/vector3.h"
#include "math/vector2.h"

typedef enum MaterialType {
  MATERIAL_TYPE_DIFFUSE,
  MATERIAL_TYPE_METAL,
  MATERIAL_TYPE_GLASS,
  MATERIAL_TYPE_EMISSIVE
} MaterialType;

struct RayHit;
typedef struct Material {
  MaterialType type;

  Color (*get_color)(struct Material* material, Vector2 uv_coordinates);
  Vector3 (*get_direction)(struct Material* material, struct RayHit rayhit, u64* state); // the return vector will be normalized
  void (*destroy)(struct Material* material);
} Material;
