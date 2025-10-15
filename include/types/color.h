#pragma once

#include "types/base_types.h"

typedef struct Color {
  f32 red, green, blue;
} Color;

typedef struct ColorRGB {
  u8 red, green, blue;
} ColorRGB;

Color color_add(Color a, Color b);
Color color_mulitply(Color a, Color b);
Color color_scale(Color a, f32 scalar);

ColorRGB color_convert_to_rgb(Color color);
