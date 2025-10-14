#include "camera.h"

#include <stdlib.h>
#include <stdio.h>

#include "types/base_types.h"
#include "types/color.h"

Camera camera_create(u32 width, u32 height) {
  Camera camera = {0};

  camera.framebuffer = (Color*) malloc(sizeof(Color) * width * height);
  if (!camera.framebuffer) {
    fprintf(stderr, "[ERROR] [CAMERA] Failed to allocate memory for framebuffer!\n");
    return (Camera) {0};
  }
  camera.width = width;
  camera.height = height;

  return camera;
}
