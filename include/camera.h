#pragma once

#include "types/color.h"
#include "types/base_types.h"

typedef struct Camera {
  Color* framebuffer;
  u32 width, height;
} Camera;

Camera camera_create(u32 width, u32 height);
