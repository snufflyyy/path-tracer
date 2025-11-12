#include "textures/solid_color.h"
#include "textures/texture.h"

#include <stdlib.h>
#include <stdio.h>

static Color get_color(Texture* texture, Vector2 uv_coordinates);
static void destroy(Texture* texture);

TextureSolidColor* texture_solid_color_create(Color color) {
  TextureSolidColor* texture = (TextureSolidColor*) malloc(sizeof(TextureSolidColor));
  if (!texture) {
    fprintf(stderr, "[ERROR] [TEXTURE] [SOLID COLOR] Failed to allocate memory for solid color texture!\n");
    return NULL;
  }

  texture->texture = (Texture) { TEXTURE_TYPE_SOLID_COLOR, get_color, destroy };
  texture->color = color;

  return texture;
}

static inline Color get_color(Texture* texture, Vector2 uv_coordinates) {
  return texture_solid_color_get((TextureSolidColor*) texture);
}

static inline void destroy(Texture* texture) {
  texture_solid_color_destroy((TextureSolidColor*) texture);
}

inline Color texture_solid_color_get(TextureSolidColor* solid_color) {
  return solid_color->color;
}

cJSON* texture_solid_color_json_create(TextureSolidColor* solid_color) {
  cJSON* texture_json = cJSON_CreateObject();
  if (!texture_json) { goto error; }

  if (!cJSON_AddNumberToObject(texture_json, "type", TEXTURE_TYPE_SOLID_COLOR)) { goto error; }

  cJSON* color = cJSON_AddArrayToObject(texture_json, "color");
  if (!color) { goto error; }

  for (usize i = 0; i < 3; i++) {
    cJSON* element = cJSON_CreateNumber(solid_color->color.data[i]);
    if (!element) { goto error; }

    if (!cJSON_AddItemToArray(color, element)) { goto error; }
  }

  return texture_json;

error:
  fprintf(stderr, "[ERROR] [TEXTURE] [SOLID COLOR] Failed create JSON object!\n");
  cJSON_Delete(texture_json);
  return NULL;
}

TextureSolidColor* texture_solid_color_json_parse(cJSON* solid_color_json) {
  cJSON* color = cJSON_GetObjectItemCaseSensitive(solid_color_json, "color");
  if (!color || !cJSON_IsArray(color)) { goto error; }

  return texture_solid_color_create((Color) { cJSON_GetNumberValue(cJSON_GetArrayItem(color, 0)), cJSON_GetNumberValue(cJSON_GetArrayItem(color, 1)), cJSON_GetNumberValue(cJSON_GetArrayItem(color, 2)) });

error:
  fprintf(stderr, "[ERROR] [TEXTURE] [SOLID COLOR] Failed parse JSON object!\n");
  return NULL;
}

void texture_solid_color_destroy(TextureSolidColor* solid_color) {
  free(solid_color);
}
