#include "textures/image.h"
#include "cJSON.h"
#include "math/vector2.h"
#include "textures/texture.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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

void texture_image_destroy(TextureImage* image) {
  free(image->pixels);
  free(image);
}
