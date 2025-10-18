#include "camera.h"
#include "hittables/hittable.h"
#include "hittables/sphere.h"
#include "math/vector3.h"
#include "types/base_types.h"
#include "types/material.h"
#include <float.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <cimgui_impl.h>

#include "gui/gui.h"
#include "gui/texture.h"
#include "gui/window.h"
#include "types/color.h"
#include "image.h"

GUI gui_create(u32 width, u32 height) {
  GUI gui = {0};

  gui.window = window_create(width, height, "Path Tracer");
  gui.texture = texture_create();
  gui.framebufferRGB = (ColorRGB*) malloc(sizeof(ColorRGB) * (width * height));
  if (!gui.framebufferRGB) {
    fprintf(stderr, "[ERROR] [GUI] Failed to allocate memory for RGB framebuffer!\n");
    return (GUI) {0};
  }

  gui.show_camera_window = true;
  gui.show_world_window = true;
  gui.show_render_window = true;

  return gui;
}

void gui_update(GUI* gui, Camera* camera, World* world) {
  window_update(gui->window);

  usize framebuffer_length = camera->width * camera->height;
  for (usize i = 0; i < framebuffer_length; i++) {
    Color color = color_scale(camera->framebuffer[i], 1.0f / camera->sample_count);
    if (camera->gamma_correction) {
      color = color_linear_to_gamma(color);
    }

    gui->framebufferRGB[i] = color_convert_to_rgb(color);
  }
  texture_set_colorRGB_buffer(gui->texture, gui->framebufferRGB, camera->width, camera->height);

  bool reset_camera_framebuffer = false;
  window_imgui_begin_frame();

  igBeginMainMenuBar();
  if (igBeginMenu("File", true)) {
    if (igMenuItem_Bool("Export JPG", NULL, false, true)) {
      camera_render_export(camera, world);
      image_create("output.jpg", camera->framebuffer, camera->sample_count, camera->width, camera->height);

      printf("[INFO] [EXPORT] Saved render to output.jpg\n");
    }
    if (igMenuItem_Bool("Exit", NULL, false, true)) {
      window_set_is_running(gui->window, false);
    }
    igEndMenu();
  }
  if (igBeginMenu("Window", true)) {
    igMenuItem_BoolPtr("Show Camera Window", NULL, &gui->show_camera_window, true);
    igMenuItem_BoolPtr("Show World Window", NULL, &gui->show_world_window, true);
    igMenuItem_BoolPtr("Show Render Window", NULL, &gui->show_render_window, true);
    igEndMenu();
  }
  igEndMainMenuBar();

  igDockSpaceOverViewport(igGetID_Str("dockspace"), NULL, ImGuiDockNodeFlags_PassthruCentralNode, NULL);

  if (gui->show_camera_window) {
    igBegin("Camera", &gui->show_camera_window, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
      igSeparatorText("Statistics");

      igText("Resolution: %dx%d (%d pixels)", camera->width, camera->height, camera->width * camera->height);
      igText("Frames per second: %0.2f", igGetIO_Nil()->Framerate);
      igText("Samples: %d", camera->sample_count);

      igSeparatorText("Settings");

      f32 position[3] = { camera->position.x, camera->position.y, camera->position.z };
      if (igDragFloat3("Camera Position", position, 0.1f, -1000.0f, 1000.0f, "%0.2f", 0)) {
        camera->position = (Vector3) { position[0], position[1], position[2] };
        reset_camera_framebuffer = true;  
      }
      if (igDragFloat("Focal Length", &camera->focal_length, 0.05f, 0.0f, 1000.0f, "%0.2f", 0)) {
        reset_camera_framebuffer = true;
      }

      igSeparatorText("Rendering Settings");

      igInputInt("Sample Limit", (s32*) &camera->sample_limit, 1, 1, 0);
      igSliderInt("Thread Count", (s32*) &camera->thread_count, 1, MAX_THREAD_COUNT, "%u", 0);
      igCheckbox("Render", &camera->render);
      igCheckbox("Gamma Correct", &camera->gamma_correction);
      if (igSmallButton("Reset framebuffer")) { reset_camera_framebuffer = true; }
    igEnd();
    }

  if (gui->show_world_window) {
    igBegin("World", &gui->show_world_window, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
      igSeparatorText("Settings");

      igInputInt("Max Ray Bounces", (s32*) &world->max_ray_bounces, 1, 1, 0);

      f32 sky_color[3] = { world->sky_color.red, world->sky_color.green, world->sky_color.blue };
      if (igColorEdit3("Sky Color", sky_color, ImGuiColorEditFlags_NoPicker)) {
        world->sky_color = (Color) { sky_color[0], sky_color[1], sky_color[2] };
        reset_camera_framebuffer = true;
      }

      igSeparatorText("Scene");

      for (usize i = 0; i < world->hittables_count; i++) {
        igPushID_Int(i);
        if (igCollapsingHeader_BoolPtr("Sphere", NULL, 0)) {
          igSeparatorText("Settings");

          f32 position[3] = { world->hittables[i]->position->x, world->hittables[i]->position->y,  world->hittables[i]->position->z };
          if (igDragFloat3("Sphere Position", position, 0.1f, -1000.0f, 1000.0f, "%0.2f", 0)) {
            *world->hittables[i]->position = (Vector3) { position[0], position[1], position[2] };
            reset_camera_framebuffer = true;
          }
          if (igDragFloat("Radius", &((Sphere*) world->hittables[i])->radius, 0.1f, -1000.0f, 1000.0f, "%0.2f", 0)) {
            reset_camera_framebuffer = true;
          }

          igSeparator();

          if (igSmallButton("Remove")) {
            world_remove(world, i);
            reset_camera_framebuffer = true;
          }
          igSameLine(0, igGetStyle()->ItemSpacing.x);
          if (igSmallButton("Duplicate")) {
            Sphere* sphere = (Sphere*) world->hittables[i];
            world_add(world, (Hittable*) sphere_create(sphere->position, sphere->radius, sphere->material));
            reset_camera_framebuffer = true;
          }

          igSeparatorText("Material");

          if (igCheckbox("Glass", &((Sphere*) world->hittables[i])->material.glass)) {
            ((Sphere*) world->hittables[i])->material.roughness = 0.0f;
            reset_camera_framebuffer = true;
          }
          if (((Sphere*) world->hittables[i])->material.glass) {
            if (igDragFloat("Refraction Index", &((Sphere*) world->hittables[i])->material.refraction_index, 0.1f, 0.0f, 100.0f, "%0.2f", 0)) {
              reset_camera_framebuffer = true;
            }
          }
          f32 albedo_color[3] = { ((Sphere*) world->hittables[i])->material.albedo.red, ((Sphere*) world->hittables[i])->material.albedo.green, ((Sphere*) world->hittables[i])->material.albedo.blue };
          if (igColorEdit3("Albedo", albedo_color, 0)) {
            ((Sphere*) world->hittables[i])->material.albedo = (Color) { albedo_color[0], albedo_color[1], albedo_color[2] };
            reset_camera_framebuffer = true;
          }
          if (igDragFloat("Roughness", &((Sphere*) world->hittables[i])->material.roughness, 0.05f, 0.0f, 1.0f, "%0.2f", 0)) {
            reset_camera_framebuffer = true;
          }
          if (igDragFloat("Emission Strength", &((Sphere*) world->hittables[i])->material.emission_strength, 0.25f, 0.0f, 100.0f, "%0.2f", 0)) {
            reset_camera_framebuffer = true;
          }
        }
        igPopID();
      }

      igSeparator();

      if (igSmallButton("Add")) {
        world_add(world, (Hittable*) sphere_create((Vector3) { 0.0f, 0.0f, 0.0f }, 1.0f, MATERIAL_GRAY));
        camera_clear_framebuffer(camera);
      }
    igEnd();

    if (gui->show_render_window) {
      igBegin("Render", &gui->show_render_window, 0);
        igImage((ImTextureRef) { NULL, gui->texture }, (ImVec2) { camera->width, camera->height }, (ImVec2) { 1.0f, 1.0f }, (ImVec2) { 0.0f, 0.0f } );
      igEnd();
    }
  }
  window_imgui_end_frame();

  if (reset_camera_framebuffer) { camera_clear_framebuffer(camera); }
}

void gui_render(GUI* gui) {
  window_clear(gui->window);
  window_imgui_render();
}

void gui_destroy(GUI* gui) {
  free(gui->framebufferRGB);
  texture_destroy(gui->texture);
  window_destroy(gui->window);
}
