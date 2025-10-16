#include "camera.h"
#include "math/sphere.h"
#include "math/vector3.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

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

void gui_update(GUI* gui, Camera* camera, World* world) {
  window_update(gui->window);

  window_imgui_begin_frame();
  igBegin("Path Tracer", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);

  igSeparatorText("Camera");

  igText("Resolution: %dx%d (%d pixels)", camera->width, camera->height, camera->width * camera->height);
  igText("Samples: %d", camera->sample_count);
  if (igSmallButton("Reset")) {
    camera_clear_framebuffer(camera);
  }
  igCheckbox("Render", &camera->render);
  igInputInt("Thread count", &camera->thread_count, 1, 1, 0);
  f32 position[3] = { camera->position.x, camera->position.y, camera->position.z };
  if (igInputFloat3("Camera Position", position, "%0.2f", 0)) {
    camera->position = (Vector3) { position[0], position[1], position[2] };
    camera_clear_framebuffer(camera);
  }

  igSeparatorText("World");
  igInputInt("Max Ray Bounces", &world->max_ray_bounces, 1, 2, 0);
  f32 sky_color[3] = { world->sky_color.red, world->sky_color.green, world->sky_color.blue };
  if (igColorEdit3("Sky Color", sky_color, ImGuiColorEditFlags_NoPicker)) {
    world->sky_color = (Color) { sky_color[0], sky_color[1], sky_color[2] };
    camera_clear_framebuffer(camera);
  }
  for (usize i = 0; i < world->hittables_count; i++) {
    igPushID_Int(i);
    if (igCollapsingHeader_BoolPtr("Sphere", NULL, 0)) {
      f32 position[3] = { world->hittables[i].position.x, world->hittables[i].position.y,  world->hittables[i].position.z };
      if (igInputFloat3("Sphere Position", position, "%0.2f", 0)) {
        world->hittables[i].position = (Vector3) { position[0], position[1], position[2] };
        camera_clear_framebuffer(camera);
      }
      if(igInputFloat("Radius", &world->hittables[i].radius, 0.5f, 1.0f, "%0.2f", 0)) {
        camera_clear_framebuffer(camera);
      }
      if (igSmallButton("Remove")) {
        world_remove(world, i);
        camera_clear_framebuffer(camera);
      }
    }
    igPopID();
  }
  if (igSmallButton("Add")) {
    world_add(world, (Sphere) { (Vector3) { 0.0f, 0.0f, 0.0f }, 1.0f });
    camera_clear_framebuffer(camera);
  }

  igSeparatorText("Exporting");

  if (igSmallButton("Export PNG")) {
    if (strnlen(gui->export_filename_buffer, EXPORT_FILENAME_BUFFER_SIZE) > 0) {
      image_create(gui->export_filename_buffer, camera->framebuffer, camera->width, camera->height);
      printf("[INFO] [EXPORT] Exported render to %s\n", gui->export_filename_buffer);
    } else {
      fprintf(stderr, "[ERROR] [EXPORT] Failed to export image, no filename!\n");
    }
  }
  igSameLine(0, igGetStyle()->ItemSpacing.x);
  igInputTextWithHint(" ", "filename.png", gui->export_filename_buffer, EXPORT_FILENAME_BUFFER_SIZE, 0, NULL, NULL);
  igEnd();
  window_imgui_end_frame();
}

void gui_render(GUI* gui, Color* framebuffer, u32 sample_count, u32 width, u32 height) {
  usize framebuffer_length = width * height;
  for (usize i = 0; i < framebuffer_length; i++) {
    gui->framebufferRGB[i] = color_convert_to_rgb(color_scale(framebuffer[i], 1.0f / sample_count));
  }
  texture_set_colorRGB_buffer(gui->sprite.texture, gui->framebufferRGB, width, height);

  window_clear(gui->window);
  sprite_render(&gui->sprite);
  window_imgui_render();
}

void gui_destroy(GUI* gui) {
  free(gui->framebufferRGB);
}
