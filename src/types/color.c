#include "types/color.h"

inline Color color_add(Color a, Color b) {
  return (Color) { (a.red + b.red), (a.green + b.green), (a.blue + b.blue) };
}

inline Color color_mulitply(Color a, Color b) {
  return (Color) { (a.red * b.red), (a.green * b.green), (a.blue * b.blue) };
}

inline Color color_scale(Color a, f32 scalar) {
  return (Color) { a.red * scalar, a.green * scalar, a.blue * scalar };
}
