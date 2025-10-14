#pragma once

#include "types/base_types.h"

typedef struct Color {
  f32 red, green, blue;
} Color;

typedef struct ColorRGB {
  u8 red, green, blue;
} ColorRGB;

ColorRGB color_convert_to_rgb(Color *color);
