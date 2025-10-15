#include <stdlib.h>
#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS 
#include <cimgui.h>
#include <cimgui_impl.h>

#include "gui/gui.h"
#include "gui/sprite.h"
#include "gui/texture.h"
#include "gui/window.h"
#include "types/color.h"
#include "image.h"


GUI gui_create(u32 width, u32 height) {
  GUI gui = {0};

  gui.window = window_create(width, height, "Path Tracer");
  gui.sprite = sprite_create(shader_create("../shaders/shader.vert", "../shaders/shader.frag"), texture_create());
  gui.framebufferRGB = (ColorRGB*) malloc(sizeof(ColorRGB) * (width * height));
  if (!gui.framebufferRGB) {
    fprintf(stderr, "[ERROR] [GUI] Failed to allocate memory for RGB framebuffer!\n");
    return (GUI) {0};
  }

  return gui;
}

void gui_update(GUI* gui, Camera* camera) {
  window_update(gui->window);

  for (usize i = 0; i < camera->width * camera->height; i++) {
    gui->framebufferRGB[i] = color_convert_to_rgb(camera->framebuffer[i]);
  }
  texture_set_colorRGB_buffer(gui->sprite.texture, gui->framebufferRGB, camera->width, camera->height);
}

void gui_render(GUI* gui, Camera* camera) {
  window_clear(gui->window);
  sprite_render(&gui->sprite);

  window_imgui_begin_frame();
  igBegin("Path Tracer", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
  if (igSmallButton("Export PNG")) {
    image_create(gui->export_filename_buffer, camera->framebuffer, camera->width, camera->height);
    printf("[INFO] [EXPORT] Exported render to %s\n", gui->export_filename_buffer);
  }
  igSameLine(0, igGetStyle()->ItemSpacing.x);
  igInputTextWithHint(" ", "filename.png", gui->export_filename_buffer, EXPORT_FILENAME_BUFFER_SIZE, 0, NULL, NULL);
  igEnd();
  window_imgui_end_frame();
}

void gui_destroy(GUI* gui) {

}
