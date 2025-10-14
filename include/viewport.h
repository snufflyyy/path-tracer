#pragma once

#include "math/vector2.h"

typedef struct Viewport {
  Vector2 first_pixel;
  Vector2 size;
  Vector2 pixel_delta;
} Viewport;

Viewport viewport_create(u32 image_width, u32 image_height);
