#include "types/color.h"

#include <math.h>

inline Color color_add(Color a, Color b) {
  return (Color) { (a.red + b.red), (a.green + b.green), (a.blue + b.blue) };
}

inline Color color_mulitply(Color a, Color b) {
  return (Color) { (a.red * b.red), (a.green * b.green), (a.blue * b.blue) };
}

inline Color color_scale(Color a, f32 scalar) {
  return (Color) { a.red * scalar, a.green * scalar, a.blue * scalar };
}

inline Color color_linear_to_gamma(Color color) {
  return (Color) { sqrtf(color.red), sqrtf(color.green), sqrtf(color.blue) };
}

inline ColorRGB color_convert_to_rgb(Color color) {
  return (ColorRGB) { fmin(color.red, 1.0f) * 255.0f, fmin(color.green, 1.0f) * 255.0f, fmin(color.blue, 1.0f) * 255.0f };
}
