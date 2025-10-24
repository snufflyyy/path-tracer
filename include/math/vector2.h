#pragma once

#include "types/base_types.h"

typedef union Vector2 {
  struct { f32 x, y; };
  f32 data[2];
} Vector2;
