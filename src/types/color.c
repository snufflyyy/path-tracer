#include "types/color.h"

inline ColorRGB color_convert_to_rgb(Color *color) {
  return (ColorRGB) { color->red * 255.0f, color->green * 255.0f, color->blue * 255.0f };
}
