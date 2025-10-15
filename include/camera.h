#pragma once

#include "math/vector3.h"
#include "types/color.h"
#include "types/base_types.h"
#include "viewport.h"
#include "world.h"

typedef struct Camera {
  Color* framebuffer;
  u32 sample_count;
  u32 width, height;

  Vector3 position;
  float focal_length;

  Viewport viewport;
} Camera;

Camera camera_create(u32 width, u32 height);
void camera_clear_framebuffer(Camera* camera);
void camera_render(Camera* camera, World* world); 
