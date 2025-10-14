#pragma once

#include "gui/window.h"
#include "gui/sprite.h"
#include "camera.h"
#include "types/color.h"

#define EXPORT_FILENAME_BUFFER_SIZE 32

typedef struct GUI {
  Window* window;
  Sprite sprite;
  ColorRGB* framebufferRGB;
  char export_filename_buffer[EXPORT_FILENAME_BUFFER_SIZE];
} GUI;

GUI gui_create(u32 width, u32 height);
void gui_update(GUI* gui, Camera* camera);
void gui_render(GUI* gui, Camera* camera);
void gui_destroy(GUI* gui);
