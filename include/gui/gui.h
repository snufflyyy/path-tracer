#pragma once

#include <stdbool.h>

#include "gui/window.h"
#include "gui/texture.h"
#include "hittables/hittable.h"
#include "types/color.h"
#include "camera.h"
#include "image.h"

// these can probably be replaced by some a macro
#define MATERIAL_TYPES_STRING "Diffuse\0Metal\0Glass\0Emissive\0"
#define TONEMAPPING_OPERATORS_STRING "Clamp\0Reinhard\0"
#define IMAGE_TYPES_STRING "HDR\0JPG\0"
#define HITTABLE_TYPES_STRING "Sphere\0Plane\0"

typedef struct GUI {
  Window* window;
  Texture texture;
  ColorRGB* framebufferRGB;

  bool show_camera_window;
  bool show_world_window;
  bool show_render_window;
  bool show_export_warning_window;

  ImageType export_image_type;

  HittableType add_type;
} GUI;

GUI gui_create(u32 width, u32 height);
void gui_update(GUI* gui, Camera* camera, World* world);
void gui_render(GUI* gui);
void gui_destroy(GUI* gui);
