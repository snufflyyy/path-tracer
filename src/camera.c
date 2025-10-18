#include "camera.h"

#include <float.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

#include "math/vector3.h"
#include "types/color.h"
#include "world.h"
#include "types/base_types.h"
#include "random.h"
#include "rayhit.h"
#include "camera.h"

static f32 reflectance(f32 cosine, f32 refraction_ratio);

static Color cast_ray(Ray ray, World* world, u64* state, u32 bounce_count) {
  if (bounce_count > world->max_ray_bounces) {
    return (Color) { 0.0f, 0.0f, 0.0f };
  }

  RayHit closest_hit = {0};
  f32 closest_t = FLT_MAX;

  for (usize i = 0; i < world->hittables_count; i++) {
    RayHit rayhit = world->hittables[i]->hit(world->hittables[i], ray);
    if (rayhit.hit && rayhit.t > 0.001f && rayhit.t < closest_t) {
      closest_hit = rayhit;
      closest_t = rayhit.t;
    }
  }

  if (closest_hit.hit) {
    Vector3 origin = closest_hit.hit_position;
    Vector3 direction = vector3_reflect(ray.direction, closest_hit.normal);
    if (closest_hit.material.glass) {
      f32 refraction_ratio = closest_hit.inside ? (1.0f / closest_hit.material.refraction_index) : closest_hit.material.refraction_index;
      f32 cos_theta = fmin(vector3_dot_product(vector3_scale(vector3_normalize(ray.direction), -1.0f), closest_hit.normal), 1.0f);
      f32 sin_theta = sqrtf(1.0f - (cos_theta * cos_theta));

      if (refraction_ratio * sin_theta < 1.0f || reflectance(cos_theta, refraction_ratio) > random_f32(state)) {
        direction = vector3_refract(vector3_normalize(ray.direction), closest_hit.normal, refraction_ratio);
      }
    }

    Color color = color_add(closest_hit.material.albedo, color_scale(closest_hit.material.albedo, closest_hit.material.emission_strength)); // emission
    Ray new_ray = (Ray) { origin, vector3_add(direction, vector3_scale(vector3_random_unit_vector(state), closest_hit.material.roughness)) };

    return color_mulitply(color, cast_ray(new_ray, world, state, bounce_count + 1));
  }

  return world->sky_color;
}

static f32 reflectance(f32 cosine, f32 refraction_ratio) {
  f32 reflectance = (1.0f - refraction_ratio) / (1.0f + refraction_ratio);
  reflectance *= reflectance;
  return reflectance + (1.0f - reflectance) * powf(1.0f - cosine, 5.0f);
}

Camera* camera_create(u32 width, u32 height, World* world) {
  Camera* camera = (Camera*) malloc(sizeof(Camera));

  camera->position = (Vector3) { 0.0f, 0.0f, 0.0f };
  camera->focal_length = 1.0f;

  camera->viewport = viewport_create(width, height);

  u32 framebuffer_length = width * height;
  camera->framebuffer = (Color*) malloc(sizeof(Color) * framebuffer_length);
  if (!camera->framebuffer) {
    fprintf(stderr, "[ERROR] [CAMERA] Failed to allocate memory for framebuffer!\n");
    return NULL;
  }
  memset(camera->framebuffer, 0, sizeof(Color) * framebuffer_length);
  camera->sample_count = 0;
  camera->sample_limit = DEFAULT_SAMPLE_LIMIT;
  camera->width = width;
  camera->height = height;

  camera->render = true;
  camera->gamma_correction = false;

  camera->thread_count = DEFAULT_THREAD_COUNT;
  camera_render_worker_create(camera, world);

  return camera;
}

void* camera_render_worker_work(void* thread_data) {
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

    for (usize y = data->start_y; y < data->end_y; y++) {
      for (usize x = data->start_x; x < data->end_x; x++) {
        usize i = (y * data->camera->width + x);

        f32 direction_x = data->camera->viewport.first_pixel.x + (data->camera->viewport.pixel_delta.x * (x + (random_f32(&data->state) - 0.5f)));
        f32 direction_y = data->camera->viewport.first_pixel.y + (data->camera->viewport.pixel_delta.y * (y + (random_f32(&data->state) - 0.5f)));
        Vector3 direction = { direction_x, direction_y, -data->camera->focal_length };
        data->camera->framebuffer[i] = color_add(data->camera->framebuffer[i], cast_ray((Ray) { data->camera->position, direction }, data->world, &data->state, 0)); 
      }
    }

    data->work_ready = false;
    data->work_done = true;
    pthread_cond_signal(&data->cond);
    pthread_mutex_unlock(&data->lock);
  }

  return NULL;
}

void camera_render_worker_create(Camera* camera, World* world) {
  for (usize i = 0; i < MAX_THREAD_COUNT; i++) {
    u64 state = time(NULL) * (88172645463325252ULL + i); // probably find a like correct way of doing this
    camera->render_workers[i].thread_data = (CameraRenderWorkerData) {
      .alive = true,
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

void camera_render_worker_destroy(Camera* camera) {
  for (usize i = 0; i < MAX_THREAD_COUNT; i++) {
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

void camera_clear_framebuffer(Camera* camera) {
  memset(camera->framebuffer, 0, sizeof(Color) * camera->width * camera->height);
  camera->sample_count = 0;
}

void camera_render(Camera* camera, World* world) {
  if (!camera->render || camera->sample_count >= camera->sample_limit) { return; }

  for (usize i = 0; i < camera->thread_count; i++) {
    usize index_delta = (camera->height / camera->thread_count);

    pthread_mutex_lock(&camera->render_workers[i].thread_data.lock);
    camera->render_workers[i].thread_data.start_y = (i * index_delta);
    camera->render_workers[i].thread_data.end_y = (i * index_delta) + index_delta;
    pthread_mutex_unlock(&camera->render_workers[i].thread_data.lock);

    camera_render_worker_render(&camera->render_workers[i]);
  }
  for (usize i = 0; i < camera->thread_count; i++) {
    camera_render_worker_wait(&camera->render_workers[i]);
  }

  camera->sample_count++;
}

void camera_destroy(Camera* camera) {
  camera_render_worker_destroy(camera);
  free(camera->framebuffer);
  free(camera);
}
