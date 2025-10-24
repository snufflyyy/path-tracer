#include "image.h"

#include <stdbool.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "camera.h"
#include "types/base_types.h"
#include "types/color.h"

void image_create_jpg(const char* filename, Camera* camera) {
  usize framebuffer_length = camera->width * camera->height;
  ColorRGB* framebufferRGB = (ColorRGB*) malloc(sizeof(ColorRGB) * framebuffer_length);
  if (!framebufferRGB) {
    fprintf(stderr, "[ERROR] [IMAGE] Failed to allocate memory for RGB framebuffer!\n");
    return;
  }

  for (usize i = 0; i < framebuffer_length; i++) {
    framebufferRGB[i] = tonemapping(camera->tonemapping_operator, color_scale(camera->framebuffer[i], 1.0f / camera->sample_count));
  }

  stbi_flip_vertically_on_write(true);
  stbi_write_jpg(filename, camera->width, camera->height, 3, framebufferRGB, 100);

  free(framebufferRGB);
}

void image_create_hdr(const char* filename, Camera* camera) {
  usize framebuffer_length = camera->width * camera->height;
  Color* framebuffer = (Color*) malloc(sizeof(Color) * framebuffer_length);
  if (!framebuffer) {
    fprintf(stderr, "[ERROR] [IMAGE] Failed to allocate memory for floating-point framebuffer!\n");
    return;
  }

  for (usize i = 0; i < framebuffer_length; i++) {
    framebuffer[i] = color_scale(camera->framebuffer[i], 1.0f / camera->sample_count);
  }

  stbi_flip_vertically_on_write(true);
  stbi_write_hdr(filename, camera->width, camera->height, 3, (f32*) framebuffer);

  free(framebuffer);
}
