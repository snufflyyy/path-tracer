#include "camera.h"

#include <float.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "materials/material.h"
#include "math/vector2.h"
#include "math/vector3.h"
#include "tonemapping.h"
#include "types/color.h"
#include "world.h"
#include "types/base_types.h"
#include "random.h"
#include "types/rayhit.h"
#include "camera.h"

static RayHit cast_indirect(Ray ray, World* world, u64* state);
static RayHit cast_direct(Ray ray, World* world, u64* state);

static Color cast_ray(Ray ray, World* world, u64* state, u32 bounce_count) {
  Color result = world->sky_color;

  usize max_bounces = world->max_ray_bounces;
  if (!world->indirect_light_sampling) { max_bounces = 1; }

  for (usize i = 0; i < max_bounces; i++) {
    RayHit indirect = cast_indirect(ray, world, state);
    if (!indirect.hit) { return result; }

    result = color_mulitply(result, indirect.material->get_color(indirect.material));

    ray = (Ray) {
      .origin = indirect.hit_position,
      .direction = indirect.material->get_direction(indirect.material, indirect, state)
    };

    if (world->direct_light_sampling) {
      RayHit direct = cast_direct(ray, world, state);
      if (!direct.hit) { return result; }
      result = color_mulitply(result, direct.material->get_color(direct.material));
    }
  }

  return result;
}

static RayHit cast_indirect(Ray ray, World* world, u64* state) {
  RayHit closest_hit = {0};
  f32 closest_t = FLT_MAX;

  for (usize i = 0; i < world->hittables_count; i++) {
    RayHit rayhit = world->hittables[i]->hit(world->hittables[i], ray);
    if (rayhit.hit && rayhit.t > 0.001f && rayhit.t < closest_t) {
      closest_hit = rayhit;
      closest_t = rayhit.t;
    }
  }

  return closest_hit;
}

static RayHit cast_direct(Ray ray, World* world, u64* state) {
  return (RayHit) {0};
}

Camera* camera_create(u32 width, u32 height, World* world) {
  Camera* camera = (Camera*) malloc(sizeof(Camera));
  if (!camera) {
    fprintf(stderr, "[ERROR] [CAMERA] Failed to allocate memory for camera!\n");
    return NULL;
  }

  camera->position = (Vector3) { 0.0f, 0.0f, 0.0f };
  camera->focal_length = 1.0f;

  camera->viewport = viewport_create(width, height);

  camera->width = width;
  camera->height = height;

  u32 framebuffer_length = width * height;
  camera->framebuffer = (Color*) malloc(sizeof(Color) * framebuffer_length);
  if (!camera->framebuffer) {
    fprintf(stderr, "[ERROR] [CAMERA] Failed to allocate memory for framebuffer!\n");
    return NULL;
  }
  memset(camera->framebuffer, 0, sizeof(Color) * framebuffer_length);
  camera->sample_count = 0;
  camera->sample_limit = DEFAULT_SAMPLE_LIMIT;
  camera->tonemapping_operator = (ToneMappingOperator) { CLAMP, 1.0f };

  camera->render = true;

  camera->thread_count = DEFAULT_THREAD_COUNT;
  camera_render_workers_create(camera, world);

  return camera;
}

static void* camera_render_worker_work(void* thread_data) {
  CameraRenderWorkerData* data = (CameraRenderWorkerData*) thread_data;

  while (true) {
    pthread_mutex_lock(&data->lock);
    while (!data->work_ready) {
      pthread_cond_wait(&data->cond, &data->lock);
    }

    if (!data->alive) {
      pthread_mutex_unlock(&data->lock);
      break;
    }

    usize sample_count = 1;
    if (data->export_mode) {
      sample_count = data->camera->sample_limit;
    }

    data->work_ready = false;

    Camera* camera = data->camera;
    World* world = data->world;
    u64* state = &data->state;
    usize start_x = data->start_x, end_x = data->end_x;
    usize start_y = data->start_y, end_y = data->end_y;
    pthread_mutex_unlock(&data->lock);

    for (usize sample = 0; sample < sample_count; sample++) {
      for (usize y = start_y; y < end_y; y++) {
        for (usize x = start_x; x < end_x; x++) {
          usize i = (y * camera->width + x);

          f32 direction_x = camera->viewport.first_pixel.x + (camera->viewport.pixel_delta.x * (x + (random_f32(state) - 0.5f)));
          f32 direction_y = camera->viewport.first_pixel.y + (camera->viewport.pixel_delta.y * (y + (random_f32(state) - 0.5f)));
          Vector3 direction = { direction_x, direction_y, -camera->focal_length };
          data->camera->framebuffer[i] = color_add(camera->framebuffer[i], cast_ray((Ray) { camera->position, direction }, world, state, 0));
        }
      }
    }

    pthread_mutex_lock(&data->lock);
    data->work_done = true;
    pthread_cond_signal(&data->cond);
    pthread_mutex_unlock(&data->lock);
  }

  return NULL;
}

void camera_render_workers_create(Camera* camera, World* world) {
  for (usize i = 0; i < camera->thread_count; i++) {
    u64 state = time(NULL) * (88172645463325252ULL + i); // probably find a like correct way of doing this
    camera->render_workers[i].thread_data = (CameraRenderWorkerData) {
      .alive = true,
      .export_mode = false,
      .work_ready = false,
      .work_done = false,

      .camera = camera,
      .world = world,
      .state = state,

      .start_x = 0,
      .end_x = camera->width,
    };
    pthread_mutex_init(&camera->render_workers[i].thread_data.lock, NULL);
    pthread_cond_init(&camera->render_workers[i].thread_data.cond, NULL);
    pthread_create(&camera->render_workers[i].thread, NULL, camera_render_worker_work, &camera->render_workers[i].thread_data);
  }
}

void camera_change_resolution(Camera* camera, u32 new_width, u32 new_height) {
  usize framebuffer_length = (new_width * new_height);
  Color* temp = (Color*) realloc(camera->framebuffer, sizeof(Color) * framebuffer_length);
  if (!temp) {
    fprintf(stderr, "[ERROR] [CAMERA] Failed to reallocate memory for framebuffer!\n");
    return;
  }
  camera->framebuffer = temp;
  camera->width = new_width;
  camera->height = new_height;

  camera->viewport = viewport_create(new_width, new_height);

  camera_clear_framebuffer(camera);
}

inline void camera_clear_framebuffer(Camera* camera) {
  memset(camera->framebuffer, 0, sizeof(Color) * camera->width * camera->height);
  camera->sample_count = 0;
}

void camera_render_frame(Camera* camera, World* world) {
  if (!camera->render || camera->sample_count >= camera->sample_limit) { return; }

  for (usize i = 0; i < camera->thread_count; i++) {
    usize index_delta = (camera->height / camera->thread_count);

    pthread_mutex_lock(&camera->render_workers[i].thread_data.lock);
    camera->render_workers[i].thread_data.start_y = (i * index_delta);
    camera->render_workers[i].thread_data.end_y = (i * index_delta) + index_delta;
    if (i == camera->thread_count - 1) {
      camera->render_workers[i].thread_data.end_y = camera->height;
    }
    pthread_mutex_unlock(&camera->render_workers[i].thread_data.lock);

    camera_render_worker_render(&camera->render_workers[i]);
  }

  for (usize i = 0; i < camera->thread_count; i++) {
    camera_render_worker_wait(&camera->render_workers[i]);
  }

  camera->sample_count++;
}

void camera_render_export(Camera* camera, World* world) {
  camera_clear_framebuffer(camera);
  for (usize i = 0; i < camera->thread_count; i++) {
    usize index_delta = (camera->height / camera->thread_count);

    pthread_mutex_lock(&camera->render_workers[i].thread_data.lock);
    camera->render_workers[i].thread_data.export_mode = true;
    camera->render_workers[i].thread_data.start_y = (i * index_delta);
    camera->render_workers[i].thread_data.end_y = (i * index_delta) + index_delta;
    if (i == camera->thread_count - 1) {
      camera->render_workers[i].thread_data.end_y = camera->height;
    }
    pthread_mutex_unlock(&camera->render_workers[i].thread_data.lock);

    camera_render_worker_render(&camera->render_workers[i]);
  }

  for (usize i = 0; i < camera->thread_count; i++) {
    camera_render_worker_wait(&camera->render_workers[i]);
    camera->render_workers[i].thread_data.export_mode = false;
  }

  camera->sample_count = camera->sample_limit;
}

void camera_render_worker_render(CameraRenderWorker* worker) {
  pthread_mutex_lock(&worker->thread_data.lock);

  worker->thread_data.work_done = false;
  worker->thread_data.work_ready = true;
  pthread_cond_signal(&worker->thread_data.cond);
  pthread_mutex_unlock(&worker->thread_data.lock);
}

void camera_render_worker_wait(CameraRenderWorker* worker) {
  pthread_mutex_lock(&worker->thread_data.lock);
  while (!worker->thread_data.work_done) {
    pthread_cond_wait(&worker->thread_data.cond, &worker->thread_data.lock);
  }
  pthread_mutex_unlock(&worker->thread_data.lock);
}

void camera_render_workers_destroy(Camera* camera) {
  for (usize i = 0; i < camera->thread_count; i++) {
    pthread_mutex_lock(&camera->render_workers[i].thread_data.lock);
    camera->render_workers[i].thread_data.alive = false;
    camera->render_workers[i].thread_data.work_ready = true;
    pthread_cond_signal(&camera->render_workers[i].thread_data.cond);
    pthread_mutex_unlock(&camera->render_workers[i].thread_data.lock);

    pthread_join(camera->render_workers[i].thread, NULL);

    pthread_mutex_destroy(&camera->render_workers[i].thread_data.lock);
    pthread_cond_destroy(&camera->render_workers[i].thread_data.cond);
  }
}

void camera_destroy(Camera* camera) {
  camera_render_workers_destroy(camera);
  free(camera->framebuffer);
  free(camera);
}
