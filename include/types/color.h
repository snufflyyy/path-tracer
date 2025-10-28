#pragma once

#include "types/base_types.h"

typedef union Color {
  struct { f32 red, green, blue; };
  f32 data[3];
} Color;

typedef union ColorRGB {
  struct { u8 red, green, blue; };
  u8 data[3];
} ColorRGB;

Color color_add(Color a, Color b);
Color color_mulitply(Color a, Color b);
Color color_scale(Color a, f32 scalar);

ColorRGB color_convert_to_rgb(Color color);
