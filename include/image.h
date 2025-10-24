#pragma once

#include "camera.h"

typedef enum ImageType {
  HDR,
  JPG
} ImageType;

void image_create_jpg(const char* filename, Camera* camera);
void image_create_hdr(const char* filename, Camera* camera);
