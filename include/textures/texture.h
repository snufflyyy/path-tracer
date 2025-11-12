#pragma once

#include <cJSON.h>

#include "types/color.h"
#include "math/vector2.h"

typedef enum TextureType {
  TEXTURE_TYPE_SOLID_COLOR,
  TEXTURE_TYPE_IMAGE
} TextureType;

typedef struct Texture {
  TextureType type;
  Color (*get_color)(struct Texture* texture, Vector2 uv_coordinates);
  void (*destroy)(struct Texture* texture);
} Texture;

cJSON* texture_json_create(Texture* texture);
Texture* texture_json_parse(cJSON* texture_json);
