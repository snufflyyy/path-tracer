#pragma once

#include <pthread.h>

#include "math/vector3.h"
#include "types/color.h"
#include "types/base_types.h"
#include "viewport.h"
#include "world.h"

#define DEFAULT_THREAD_COUNT 16

typedef struct CameraRenderThreadData {
  struct Camera* camera;
  World* world;
  usize start_x, end_x;
  usize start_y, end_y;
  u64 state;
} CameraRenderThreadData;

typedef struct Camera {
  Color* framebuffer;
  u32 sample_count;
  u32 width, height;

  bool render;

  pthread_t* threads;
  CameraRenderThreadData* thread_data;
  u32 thread_count;

  Vector3 position;
  float focal_length;

  Viewport viewport;
} Camera;

Camera camera_create(u32 width, u32 height);
void camera_create_worker_threads(Camera* camera, World* world);
void camera_destroy_worker_threads(Camera* camera);
void camera_clear_framebuffer(Camera* camera);
void camera_render(Camera* camera, World* world); 
