#include "camera.h"

#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <string.h>

#include "math/sphere.h"
#include "math/vector3.h"
#include "rayhit.h"
#include "types/base_types.h"
#include "types/color.h"
#include "viewport.h"
#include "world.h"

static Color cast_ray(Ray ray, World* world, u32 bounce_count) {
  if (bounce_count + 1 >= RAY_MAX_BOUNCES) {
    return (Color) { 0.0f, 0.0f, 0.0f };
  }

  RayHit closest_hit = {0};
  f32 closest_t = FLT_MAX;

  for (usize i = 0; i < world->sphere_count; i++) {
    RayHit rayhit = sphere_ray_hit(&world->spheres[i], &ray);
    if (rayhit.hit) {
      closest_hit = rayhit;
      closest_t = rayhit.t;
    }
  }

  if (closest_hit.hit) {
    Ray new_ray = (Ray) { closest_hit.hit_position, vector3_random_in_hemisphere(closest_hit.normal) };
    return color_mulitply(closest_hit.color, cast_ray(new_ray, world, bounce_count + 1));
  }

  return (Color) { 0.5f, 0.7f, 0.9f };
}

Camera camera_create(u32 width, u32 height) {
  Camera camera = {0};

  camera.framebuffer = (Color*) malloc(sizeof(Color) * width * height);
  if (!camera.framebuffer) {
    fprintf(stderr, "[ERROR] [CAMERA] Failed to allocate memory for framebuffer!\n");
    return (Camera) {0};
  }
  camera.width = width;
  camera.height = height;

  camera.position = (Vector3) { 0.0f, 0.0f, 0.0f };
  camera.focal_length = 1.0f;

  camera.viewport = viewport_create(width, height);

  return camera;
}

void camera_render(Camera* camera, World* world) {
  memset(camera->framebuffer, 0, camera->width * camera->height);

  for (usize y = 0; y < camera->height; y++) {
    for (usize x = 0; x < camera->width; x++) {
      usize i = y * camera->width + x;
      Vector3 direction = vector3_normalized((Vector3) { camera->viewport.first_pixel.x + (camera->viewport.pixel_delta.x * x), camera->viewport.first_pixel.y + (camera->viewport.pixel_delta.y * y), camera->focal_length });
      camera->framebuffer[i] = cast_ray((Ray) { camera->position, direction }, world, 0);
    }
  }
}
