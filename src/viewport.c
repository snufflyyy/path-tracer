#include "viewport.h"
#include "math/vector2.h"

Viewport viewport_create(u32 image_width, u32 image_height) {
  Viewport viewport = {0};

  viewport.size = (Vector2) { (f32) image_width / (f32) image_height, 1.0f };
  viewport.first_pixel = (Vector2) { -(viewport.size.x / 2.0f), -(viewport.size.y / 2.0f) };
  viewport.pixel_delta = (Vector2) { viewport.size.x / (f32) image_width, viewport.size.y / (f32) image_height };

  return viewport;
}
