#pragma once

#include <stdbool.h>

#include <cJSON.h>

#include "textures/texture.h"
#include "types/color.h"
#include "types/base_types.h"
#include "gui/texture.h"

#define TEXTURE_IMAGE_INVALID_PATH "../assets/invalid.png"

typedef struct TextureImage {
  Texture texture;

  const char* path_to_image;
  Color* pixels;
  u32 width, height;

  GLTexture gl_texture;
} TextureImage;

TextureImage* texture_image_create(const char* path);
Color texture_image_get(TextureImage* image, Vector2 uv_coordinates);
void texture_image_change_image(TextureImage* texture, const char* path);

bool texture_image_gui_edit(TextureImage* image);

cJSON* texture_image_json_create(TextureImage* image);
TextureImage* texture_image_json_parse(cJSON* image_json);

void texture_image_destroy(TextureImage* image);
