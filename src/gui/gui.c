#include "gui/gui.h"

#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <cimgui_impl.h>

#include "image.h"
#include "hittables/sphere.h"
#include "materials/material.h"
#include "materials/diffuse.h"
#include "materials/metal.h"
#include "materials/glass.h"
#include "materials/emission.h"

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
  gui.show_export_warning_window = false;

  return gui;
}

static void gui_update_main_menu_bar(GUI* gui);
static void gui_update_window_export_warning(GUI* gui, Camera* camera, World* world);
static void gui_update_window_render(GUI* gui, Camera* camera);
static void gui_update_window_camera(GUI* gui, Camera* camera, World* world, bool* reset_camera_framebuffer);
static void gui_update_window_world(GUI* gui, World* world, bool* reset_camera_framebuffer);

void gui_update(GUI* gui, Camera* camera, World* world) {
  window_update(gui->window);

  bool reset_camera_framebuffer = false;

  window_imgui_begin_frame();

  gui_update_main_menu_bar(gui);
  if (gui->show_export_warning_window) { 
    camera->render = false;
    gui_update_window_export_warning(gui, camera, world); 
  } else {
    camera->render = true;
  }

  igDockSpaceOverViewport(igGetID_Str("dockspace"), NULL, ImGuiDockNodeFlags_PassthruCentralNode, NULL);
  if (gui->show_render_window) { gui_update_window_render(gui, camera); }
  if (gui->show_camera_window) { gui_update_window_camera(gui, camera, world, &reset_camera_framebuffer); }
  if (gui->show_world_window) { gui_update_window_world(gui, world, &reset_camera_framebuffer); }
  window_imgui_end_frame();

  if (reset_camera_framebuffer) { camera_clear_framebuffer(camera); }
}

static void gui_update_main_menu_bar(GUI* gui) {
  igBeginMainMenuBar();
    if (igBeginMenu("File", true)) {
      if (igMenuItem_Bool("Export JPG", NULL, false, true)) {
        gui->show_export_warning_window = true;
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
}

static void gui_update_window_export_warning(GUI* gui, Camera* camera, World* world) {
  igBegin("Exporting", &gui->show_export_warning_window, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
    igText("Exporting WILL freeze the GUI until the render is complete");
    igText("Are you sure you want to continue?");
    if (igSmallButton("Yes")) {
      camera_render_export(camera, world);
      image_create("output.jpg", camera->framebuffer, camera->sample_count, camera->width, camera->height);

      gui->show_export_warning_window = false;
    }

    igSameLine(0, igGetStyle()->ItemInnerSpacing.x);

    if (igSmallButton("No")) { gui->show_export_warning_window = false; }
  igEnd();
}

static void gui_update_window_render(GUI* gui, Camera* camera) {
  if (camera->render || camera->sample_count <= camera->sample_limit) {
    usize framebuffer_length = camera->width * camera->height;
    for (usize i = 0; i < framebuffer_length; i++) {
      Color color = color_scale(camera->framebuffer[i], 1.0f / camera->sample_count);
      gui->framebufferRGB[i] = color_convert_to_rgb(color);
    }
    texture_set_colorRGB_buffer(gui->texture, gui->framebufferRGB, camera->width, camera->height);
  }

  igBegin("Render", &gui->show_render_window, 0);
    igImage((ImTextureRef) { NULL, gui->texture }, (ImVec2) { camera->width, camera->height }, (ImVec2) { 0.0f, 1.0f }, (ImVec2) { 1.0f, 0.0f } );
  igEnd();
}

static void gui_update_window_camera(GUI* gui, Camera* camera, World* world, bool* reset_camera_framebuffer) {
  igBegin("Camera", &gui->show_camera_window, 0);
    igSeparatorText("Statistics");

    igText("Frames per second: %0.2f", igGetIO_Nil()->Framerate);
    igText("Samples: %d", camera->sample_count);

    igSeparatorText("Settings");

    f32 position[3] = { camera->position.x, camera->position.y, camera->position.z };
    if (igDragFloat3("Position", position, 0.1f, -1000.0f, 1000.0f, "%0.2f", 0)) {
      camera->position = (Vector3) { position[0], position[1], position[2] };
      *reset_camera_framebuffer = true;
    }
    if (igDragFloat("Focal Length", &camera->focal_length, 0.05f, 0.0f, 1000.0f, "%0.2f", 0)) {
      *reset_camera_framebuffer = true;
    }

    igSeparatorText("Rendering Settings");

    u32 resolution[2] = { camera->width, camera->height };
    if (igDragInt2("Resolution", (s32*) resolution, 1, 1, INT32_MAX, "%u", 0)) {
      camera_change_resolution(camera, resolution[0], resolution[1]);

      camera_render_workers_destroy(camera);
      camera_render_workers_create(camera, world);

      ColorRGB* temp = (ColorRGB*) realloc(gui->framebufferRGB, sizeof(ColorRGB) * (camera->width * camera->height));
      if (!temp) {
        fprintf(stderr, "[ERROR] [GUI] Failed to resize RGB framebuffer!\n");
      }

      gui->framebufferRGB = temp;
    }

    if (igInputInt("Sample Limit", (s32*) &camera->sample_limit, 1, 1, 0)) {
      if (camera->sample_count >= camera->sample_limit) {
        *reset_camera_framebuffer = true;
      }
    }
    if (igSliderInt("Thread Count", (s32*) &camera->thread_count, 1, MAX_THREAD_COUNT, "%u", 0)) {
      camera_render_workers_destroy(camera);
      camera_render_workers_create(camera, world);
    }
    igCheckbox("Render", &camera->render);
    if (igSmallButton("Reset framebuffer")) { *reset_camera_framebuffer = true; }
  igEnd();
}

static void gui_update_window_world(GUI* gui, World* world, bool* reset_camera_framebuffer) {
  bool remove_hittable = false;
  usize remove_hittable_index;

  igBegin("World", &gui->show_world_window, 0);
    igSeparatorText("Settings");

    igInputInt("Max Ray Bounces", (s32*) &world->max_ray_bounces, 1, 1, 0);

    f32 sky_color[3] = { world->sky_color.red, world->sky_color.green, world->sky_color.blue };
    if (igColorEdit3("Sky Color", sky_color, ImGuiColorEditFlags_NoPicker)) {
      world->sky_color = (Color) { sky_color[0], sky_color[1], sky_color[2] };
      *reset_camera_framebuffer = true;
    }

    igSeparatorText("Scene");

    for (usize i = 0; i < world->hittables_count; i++) {
      igPushID_Int(i);
      if (igCollapsingHeader_BoolPtr("Sphere", NULL, 0)) {
        igSeparatorText("Settings");

        f32 position[3] = { world->hittables[i]->position->x, world->hittables[i]->position->y,  world->hittables[i]->position->z };
        if (igDragFloat3("Sphere Position", position, 0.1f, -1000.0f, 1000.0f, "%0.2f", 0)) {
          *world->hittables[i]->position = (Vector3) { position[0], position[1], position[2] };
          *reset_camera_framebuffer = true;
        }
        if (igDragFloat("Radius", &((Sphere*) world->hittables[i])->radius, 0.1f, -1000.0f, 1000.0f, "%0.2f", 0)) {
          *reset_camera_framebuffer = true;
        }

        igSeparator();

        if (igSmallButton("Remove")) {
          remove_hittable = true;
          remove_hittable_index = i;
          *reset_camera_framebuffer = true;
        }

        igSeparatorText("Material");

        MaterialType type = world->hittables[i]->material->type;
        if (igCombo_Str("Material Type", (s32*) &type, MATERIAL_TYPES_STRING, 0)) {
          // this is very unsafe and only works because every CURRENT material has an albedo and it is located in the same place
          Color old_albedo = ((Diffuse*) world->hittables[i]->material)->albedo;
          world->hittables[i]->material->destroy(world->hittables[i]->material);

          switch (type) {
            case DIFFUSE: {
              world->hittables[i]->material = (Material*) diffuse_material_create(old_albedo);
            } break;
            case METAL: {
              world->hittables[i]->material = (Material*) metal_material_create(old_albedo, 0.0f);
            } break;
            case GLASS: {
              world->hittables[i]->material = (Material*) glass_material_create(old_albedo, 1.33f, 0.0f);
            } break;
            case EMISSIVE: {
              world->hittables[i]->material = (Material*) emissive_material_create(old_albedo, 0.5f);
            } break;
          }

          *reset_camera_framebuffer = true;
        }

        igSeparator();

        switch(world->hittables[i]->material->type) {
          case DIFFUSE: {
            Diffuse* diffuse = (Diffuse*) world->hittables[i]->material;

            f32 albedo[3] = { diffuse->albedo.red, diffuse->albedo.green, diffuse->albedo.blue };
            if (igColorEdit3("Albedo", albedo, 0)) {
              diffuse->albedo = (Color) { albedo[0], albedo[1], albedo[2] };
              *reset_camera_framebuffer = true;
            }
          } break;
          case METAL: {
            Metal* metal = (Metal*) world->hittables[i]->material;

            f32 albedo[3] = { metal->albedo.red, metal->albedo.green, metal->albedo.blue };
            if (igColorEdit3("Albedo", albedo, 0)) {
              metal->albedo = (Color) { albedo[0], albedo[1], albedo[2] };
              *reset_camera_framebuffer = true;
            }
            if (igDragFloat("Roughness", &metal->roughness, 0.1f, 0.0f, 1.0f, "%0.2f", 0)) {
              *reset_camera_framebuffer = true;
            }
          } break;
          case GLASS: {
            Glass* glass = (Glass*) world->hittables[i]->material;

            f32 albedo[3] = { glass->albedo.red, glass->albedo.green, glass->albedo.blue };
            if (igColorEdit3("Albedo", albedo, 0)) {
              glass->albedo = (Color) { albedo[0], albedo[1], albedo[2] };
              *reset_camera_framebuffer = true;
            }
            if (igDragFloat("Refraction Index", &glass->refraction_index, 0.1f, 0.0f, 50.0f, "%0.2f", 0)) {
              *reset_camera_framebuffer = true;
            }
            if (igDragFloat("Roughness", &glass->roughness, 0.1f, 0.0f, 1.0f, "%0.2f", 0)) {
              *reset_camera_framebuffer = true;
            }
          } break;
          case EMISSIVE: {
            Emissive* emissive = (Emissive*) world->hittables[i]->material;

            f32 albedo[3] = { emissive->albedo.red, emissive->albedo.green, emissive->albedo.blue };
            if (igColorEdit3("Albedo", albedo, 0)) {
              emissive->albedo = (Color) { albedo[0], albedo[1], albedo[2] };
              *reset_camera_framebuffer = true;
            }
            if (igDragFloat("Emission Strength", &emissive->emission_strength, 0.1f, 0.0f, 50.0f, "%0.2f", 0)) {
              *reset_camera_framebuffer = true;
            }
          } break;
        }
      }
      igPopID();
    }
    igSeparator();

    if (igSmallButton("Add")) {
      world_add(world, (Hittable*) sphere_create((Vector3) { 0.0f, 0.0f, 0.0f }, 1.0f, (Material*)  diffuse_material_create((Color) { 1.0f, 1.0f, 1.0f })));
      *reset_camera_framebuffer = true;
    }
  igEnd();

  if (remove_hittable) { world_remove(world, remove_hittable_index); }
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
