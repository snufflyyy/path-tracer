#include "tonemapping.h"

#include <math.h>

static f32 luminance(Color color); 
static Color change_luminance(Color color, f32 l_out);

// when i add more tonemapping functions, find a better way to deal with the variables for each function
inline ColorRGB tonemapping(ToneMappingOperator tonemapping_operator, Color color) {
  switch (tonemapping_operator.type) {
    case CLAMP: return tonemapping_clamp(color); break;
    case REINHARD: return tonemapping_reinhard(color, tonemapping_operator.max_white); break;
  }

  return tonemapping_clamp(color);
}

inline ColorRGB tonemapping_clamp(Color color) {
  return (ColorRGB) { fmin(color.red, 1.0f) * 255.0f, fmin(color.green, 1.0f) * 255.0f, fmin(color.blue, 1.0f) * 255.0f };
}

inline ColorRGB tonemapping_reinhard(Color color, f32 max_white) {
  f32 l_old = luminance(color);
  f32 numerator = l_old * (1.0f + (l_old / (max_white * max_white)));
  f32 l_new = numerator / (1.0f + l_old);

  return tonemapping_clamp(change_luminance(color, l_new));
}

static f32 luminance(Color color) {
  return (color.red * 0.2126f) + (color.green * 0.7152f) + (color.blue * 0.0722f);
}

static Color change_luminance(Color color, f32 l_out) {
  f32 l_in = luminance(color);
  return color_scale(color, l_out / l_in);
}
