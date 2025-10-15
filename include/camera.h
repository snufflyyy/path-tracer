#pragma once

#include "math/vector3.h"
#include "types/color.h"
#include "types/base_types.h"
#include "viewport.h"
#include "world.h"

#define RAY_MAX_BOUNCES 10

typedef struct Camera {
  Color* framebuffer;
  u32 width, height;
  u32 frame_count;

  Vector3 position;
  float focal_length;

  Viewport viewport;
} Camera;

Camera camera_create(u32 width, u32 height);
void camera_render(Camera* camera, World* world); 
