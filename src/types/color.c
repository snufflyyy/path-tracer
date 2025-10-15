#include "types/color.h"

#include <math.h>

inline Color color_add(Color a, Color b) {
  return (Color) { (a.red + b.red), (a.green + b.green), (a.blue + b.blue) };
}

Color color_mulitply(Color a, Color b) {
  return (Color) { (a.red * b.red), (a.green * b.green), (a.blue * b.blue) };
}

inline Color color_scale(Color a, f32 scalar) {
  return (Color) { a.red * scalar, a.green * scalar, a.blue * scalar };
}

inline Color color_divide_by_scalar(Color a, f32 scalar) {
  return (Color) { a.red / scalar, a.green / scalar, a.blue / scalar };
}

inline ColorRGB color_convert_to_rgb(Color color) {
  return (ColorRGB) { color.red * 255.0f, color.green * 255.0f, color.blue * 255.0f };
}
