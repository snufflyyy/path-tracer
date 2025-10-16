#include "image.h"
#include "types/color.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

void image_create(const char* filename, Color* framebuffer, u32 sample_count, u32 width, u32 height) {
  usize framebuffer_length = width * height;

  ColorRGB* framebufferRGB = (ColorRGB*) malloc(sizeof(ColorRGB) * framebuffer_length);
  if (!framebufferRGB) {
    fprintf(stderr, "[ERROR] [IMAGE] Failed to allocate memory for RGB framebuffer!\n");
    return;
  }

  for (usize i = 0; i < framebuffer_length; i++) {
    framebufferRGB[i] = color_convert_to_rgb(color_scale(framebuffer[i], 1.0f / sample_count));
  }

  stbi_flip_vertically_on_write(true);
  stbi_write_jpg(filename, width, height, 3, framebufferRGB, 100);

  free(framebufferRGB);
}
