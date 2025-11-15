#pragma once

#include <stdbool.h>

#include <cJSON.h>

#include "textures/texture.h"
#include "types/color.h"

typedef struct TextureSolidColor {
  Texture texture;

  Color color;
} TextureSolidColor;

TextureSolidColor* texture_solid_color_create(Color color);
Color texture_solid_color_get(TextureSolidColor* solid_color);

cJSON* texture_solid_color_json_create(TextureSolidColor* solid_color);
TextureSolidColor* texture_solid_color_json_parse(cJSON* solid_color_json);

bool texture_solid_color_gui_edit(TextureSolidColor* solid_color);

void texture_solid_color_destroy(TextureSolidColor* solid_color);
