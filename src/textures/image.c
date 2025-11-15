#include "textures/image.h"
#include "gui/texture.h"
#include "gui/window.h"
#include "math/vector2.h"
#include "textures/texture.h"
#include "tonemapping.h"
#include "utils/file.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <cJSON.h>
#include <nfd.h>

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <cimgui_impl.h>

static Color get_color(Texture* texture, Vector2 uv_coordinates);
static void destroy(Texture* texture);

TextureImage* texture_image_create(const char* path) {
  TextureImage* texture = (TextureImage*) malloc(sizeof(TextureImage));
  if (!texture) {
    fprintf(stderr, "[ERROR] [TEXTURE] [IMAGE] Failed to allocate memory for image texture!\n");
    return NULL;
  }

  texture->texture = (Texture) { TEXTURE_TYPE_IMAGE, get_color, destroy };

  texture->pixels = NULL;
  texture_image_change_image(texture, path);

  return texture;
}

void texture_image_change_image(TextureImage* texture, const char* path) {
  if (texture->pixels) { free(texture->pixels); }
  texture_destroy(texture->gl_texture);

  texture->path_to_image = path;

  stbi_set_flip_vertically_on_load(true);
  f32* image_data = stbi_loadf(path, (s32*) &texture->width, (s32*) &texture->height, NULL, 3);
  if (!image_data) {
    fprintf(stderr, "[ERROR] [TEXTURE] [IMAGE] Failed to load image data!\n");
    image_data = stbi_loadf(TEXTURE_IMAGE_INVALID_PATH, (s32*) &texture->width, (s32*) &texture->height, NULL, 3);
    if (!image_data) {
      free(texture);
      return;
    }
  }

  usize pixels_length = (texture->width * texture->height);
  texture->pixels = (Color*) malloc(sizeof(Color) * pixels_length);
  if (!image_data) {
    fprintf(stderr, "[ERROR] [TEXTURE] [IMAGE] Failed to allocate memory for pixels!\n");
    stbi_image_free((void*) image_data);
    free(texture);
    return;
  }

  for (usize i = 0; i < pixels_length; i++) {
    usize data_index = (i * 3);
    texture->pixels[i] = (Color) { image_data[data_index], image_data[data_index + 1], image_data[data_index + 2] };
  }

  stbi_image_free((void*) image_data);

  texture->gl_texture = texture_create();
  ColorRGB* pixelsRGB = (ColorRGB*) malloc(sizeof(ColorRGB) * pixels_length);
  if (!pixelsRGB) {
    fprintf(stderr, "[ERROR] [TEXTURE] [IMAGE] Failed to allocate memory for pixels RGB!\n");
    return;
  }

  for (usize i = 0; i < pixels_length; i++) {
    pixelsRGB[i] = tonemapping_clamp(texture->pixels[i]);
  }

  texture_set_colorRGB_buffer(texture->gl_texture, pixelsRGB, texture->width, texture->height);

  free(pixelsRGB);
}

static inline Color get_color(Texture* texture, Vector2 uv_coordinates) {
  return texture_image_get((TextureImage*) texture, uv_coordinates);
}

static inline void destroy(Texture* texture) {
  texture_image_destroy((TextureImage*) texture);
}

inline Color texture_image_get(TextureImage* image, Vector2 uv_coordinates) {
  u32 x = ((s32) (image->width * uv_coordinates.x)) % image->width;
  u32 y = ((s32) (image->height * uv_coordinates.y)) % image->height;
  return image->pixels[y * image->width + x];
}

cJSON* texture_image_json_create(TextureImage* image) {
  cJSON* texture_json = cJSON_CreateObject();
  if (!texture_json) { goto error; }

  if (!cJSON_AddNumberToObject(texture_json, "type", TEXTURE_TYPE_IMAGE)) { goto error; }
  if (!cJSON_AddStringToObject(texture_json, "path", image->path_to_image)) { goto error; }

  return texture_json;

error:
  fprintf(stderr, "[ERROR] [TEXTURE] [IMAGE] Failed create JSON object!\n");
  cJSON_Delete(texture_json);
  return NULL;
}

TextureImage* texture_image_json_parse(cJSON* image_json) {
  cJSON* path_json = cJSON_GetObjectItemCaseSensitive(image_json, "path");
  if (!path_json || !cJSON_IsString(path_json)) { goto error; }

  return texture_image_create(cJSON_GetStringValue(path_json));

error:
  fprintf(stderr, "[ERROR] [TEXTURE] [IMAGE] Failed parse JSON object!\n");
  return NULL;
}

bool texture_image_gui_edit(TextureImage* image) {
  ImVec2 image_preview_size;
  if (image->width > image->height) {
    image_preview_size = (ImVec2) { (((f32) image->width / image->height) * 128.0f), 128.0f };
  } else {
    image_preview_size = (ImVec2) { 128.0f, (((f32) image->height / image->width) * 128.0f) };
  }
  igImage((ImTextureRef) { NULL, image->gl_texture }, image_preview_size, (ImVec2) { 0.0f, 1.0f }, (ImVec2) { 1.0f, 0.0f } );

  if (igSmallButton("Change Image")) {
    nfdfilteritem_t filter_items[] = { { "Image", "jpg,png" } };
    const char* path = file_dialog_get_open(filter_items, (sizeof(filter_items) / sizeof(nfdfilteritem_t)));
    if (!path) {
      return false;
    }

    texture_image_change_image(image, path);
    file_dialog_string_destroy(path);
    return true;
  }

  return false;
}

void texture_image_destroy(TextureImage* image) {
  texture_destroy(image->gl_texture);
  free(image->pixels);
  free(image);
}
