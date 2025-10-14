#pragma once

#include "math/vector3.h"
#include "types/color.h"
#include "types/base_types.h"
#include "viewport.h"

typedef struct Camera {
  Color* framebuffer;
  u32 width, height;

  Vector3 position;
  float focal_length;

  Viewport viewport;
} Camera;

Camera camera_create(u32 width, u32 height);
void camera_render(Camera* camera);
