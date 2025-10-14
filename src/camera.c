#include "camera.h"

#include <stdlib.h>
#include <stdio.h>

#include "math/sphere.h"
#include "math/vector3.h"
#include "types/base_types.h"
#include "types/color.h"
#include "viewport.h"

static Color camera_cast_ray(Ray ray) {
  Sphere temp_sphere = { (Vector3) { 0.0f, 0.0f, -5.0f }, 1.0f };

  if (sphere_ray_hit(&temp_sphere, &ray)) {
    return (Color) { 0.0f, 0.5f, 1.0f, };
  }

  return (Color) { 0.0f, 0.0f, 0.0f };
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

void camera_render(Camera* camera) {
  for (usize y = 0; y < camera->height; y++) {
    for (usize x = 0; x < camera->width; x++) {
      Vector3 direction = (Vector3) { camera->viewport.first_pixel.x + (camera->viewport.pixel_delta.x * x), camera->viewport.first_pixel.y + (camera->viewport.pixel_delta.y * y), camera->focal_length };
      camera->framebuffer[y * camera->width + x] = camera_cast_ray((Ray) { camera->position, direction });
    }
  }
}
