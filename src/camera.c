#include "camera.h"

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <string.h>
#include <time.h>

#include "math/sphere.h"
#include "math/vector3.h"
#include "rayhit.h"
#include "types/base_types.h"
#include "types/color.h"
#include "viewport.h"
#include "world.h"

static Color cast_ray(Ray ray, World* world, u64* state, u32 bounce_count) {
  if (bounce_count + 1 >= world->max_ray_bounces) {
    return (Color) { 0.0f, 0.0f, 0.0f };
  }

  RayHit closest_hit = {0};
  f32 closest_t = FLT_MAX;

  for (usize i = 0; i < world->hittables_count; i++) {
    RayHit rayhit = sphere_ray_hit(&world->hittables[i], &ray);
    if (rayhit.hit && rayhit.t > 0.0f && rayhit.t < closest_t) {
      closest_hit = rayhit;
      closest_t = rayhit.t;
    }
  }

  if (closest_hit.hit) {
    Ray new_ray = (Ray) { closest_hit.hit_position, vector3_random_in_hemisphere(state, closest_hit.normal) };
    return color_mulitply(closest_hit.color, cast_ray(new_ray, world, state, bounce_count + 1));
  }

  return world->sky_color;
}

Camera camera_create(u32 width, u32 height) {
  Camera camera = {0};

  camera.framebuffer = (Color*) malloc(sizeof(Color) * width * height);
  if (!camera.framebuffer) {
    fprintf(stderr, "[ERROR] [CAMERA] Failed to allocate memory for framebuffer!\n");
    return (Camera) {0};
  }
  memset(camera.framebuffer, 0, sizeof(Color) * width * height);
  camera.width = width;
  camera.height = height;

  camera.render = true;

  camera.thread_count = DEFAULT_THREAD_COUNT;
  camera.threads = (pthread_t*) malloc(sizeof(pthread_t) * camera.thread_count);
  if (!camera.threads) {
    fprintf(stderr, "[ERROR] [CAMERA] Failed to allocate memory for worker threads!\n");
    return (Camera) {0};
  }

  camera.thread_data = (CameraRenderThreadData*) malloc(sizeof(CameraRenderThreadData) * camera.thread_count);
  if (!camera.thread_data) {
    fprintf(stderr, "[ERROR] [CAMERA] Failed to allocate memory for worker threads data!\n");
    return (Camera) {0};
  }

  camera.position = (Vector3) { 0.0f, 0.0f, 0.0f };
  camera.focal_length = 1.0f;

  camera.viewport = viewport_create(width, height);

  return camera;
}

void camera_clear_framebuffer(Camera* camera) {
  memset(camera->framebuffer, 0, sizeof(Color) * camera->width * camera->height);
  camera->sample_count = 0;
}

static void* camera_render_worker(void* thread_data) {
  CameraRenderThreadData* data = (CameraRenderThreadData*) thread_data;

  for (usize y = data->start_y; y < data->end_y; y++) {
    for (usize x = data->start_x; x < data->end_x; x++) {
      usize i = y * data->camera->width + x;
      Vector3 direction = (Vector3) { data->camera->viewport.first_pixel.x + (data->camera->viewport.pixel_delta.x * (x + (random_f32(&data->state) - 0.5f))), data->camera->viewport.first_pixel.y + (data->camera->viewport.pixel_delta.y * (y + (random_f32(&data->state) - 0.5f))), -data->camera->focal_length };
      data->camera->framebuffer[i] = color_add(data->camera->framebuffer[i], cast_ray((Ray) { data->camera->position, direction }, data->world, &data->state, 0));
    }
  }

  return NULL;
}

// todo these functions
void camera_create_worker_threads(Camera* camera, World* world) {}
void camera_destroy_worker_threads(Camera* camera) {}

void camera_render(Camera* camera, World* world) {
  if (!camera->render) { return; }

  usize index_delta = (camera->height / camera->thread_count);
  for (usize i = 0; i < camera->thread_count; i++) {
    u64 state = time(NULL) * (88172645463325252ULL + i);
    camera->thread_data[i] = (CameraRenderThreadData) { camera, world, 0, camera->width, (i * index_delta), (i * index_delta) + index_delta, state };
    pthread_create(&camera->threads[i], NULL, camera_render_worker, &camera->thread_data[i]);
  }

  for (usize i = 0; i < camera->thread_count; i++) {
    pthread_join(camera->threads[i], NULL);
  }
  camera->sample_count++;
}
