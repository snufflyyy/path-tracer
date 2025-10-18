#pragma once

#include <stdbool.h>
#include <pthread.h>

#include "world.h"
#include "types/base_types.h"
#include "math/vector3.h"
#include "viewport.h"
#include "types/color.h"

#define DEFAULT_SAMPLE_LIMIT 1000

#define MAX_THREAD_COUNT 16
#define DEFAULT_THREAD_COUNT 16

typedef struct CameraRenderWorkerData {
  bool alive;
  bool export_mode;
  bool work_ready;
  bool work_done;
  pthread_mutex_t lock;
  pthread_cond_t cond;

  struct Camera* camera;
  World* world;
  u64 state;

  usize start_x, end_x;
  usize start_y, end_y;
} CameraRenderWorkerData;

typedef struct CameraRenderWorker {
  pthread_t thread;
  CameraRenderWorkerData thread_data;
} CameraRenderWorker;

typedef struct Camera {
  Vector3 position;
  float focal_length;

  Viewport viewport;

  Color* framebuffer;
  u32 sample_count;
  u32 sample_limit;
  u32 width, height;

  bool render;
  bool gamma_correction;

  CameraRenderWorker render_workers[MAX_THREAD_COUNT];
  u32 thread_count;
} Camera;

Camera* camera_create(u32 width, u32 height, World* world);
void camera_render_worker_create(Camera* camera, World* world);
void camera_render_worker_render(CameraRenderWorker* worker);
void camera_render_worker_wait(CameraRenderWorker* worker);
void camera_render_worker_destroy(Camera* camera);
void camera_clear_framebuffer(Camera* camera);
void camera_render_frame(Camera* camera, World* world);
void camera_render_export(Camera* camera, World* world);
void camera_destroy(Camera* camera);
