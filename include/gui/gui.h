#pragma once

#include "gui/texture.h"
#include "gui/window.h"
#include "camera.h"
#include "types/color.h"

typedef struct GUI {
  Window* window;
  Texture texture;
  ColorRGB* framebufferRGB;

  bool show_camera_window;
  bool show_world_window;
  bool show_render_window;
} GUI;

GUI gui_create(u32 width, u32 height);
void gui_update(GUI* gui, Camera* camera, World* world);
void gui_render(GUI* gui);
void gui_destroy(GUI* gui);
