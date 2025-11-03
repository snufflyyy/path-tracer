#include "gui/gui.h"

#include <stdbool.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cimgui.h>
#include <cimgui_impl.h>

#include <nfd.h>

#include "hittables/hittable.h"
#include "image.h"
#include "hittables/sphere.h"
#include "hittables/plane.h"
#include "materials/material.h"
#include "materials/diffuse.h"
#include "materials/metal.h"
#include "materials/glass.h"
#include "materials/emissive.h"
#include "math/vector2.h"
#include "math/vector3.h"
#include "tonemapping.h"
#include "world.h"

static void gui_update_main_menu_bar(GUI* gui);
static void gui_update_window_export_warning(GUI* gui, Camera* camera, World* world);
static void gui_update_window_render(GUI* gui, Camera* camera);
static void gui_update_window_camera(GUI* gui, Camera* camera, World* world, bool* reset_camera_framebuffer);
static void gui_update_window_world(GUI* gui, World* world, Camera* camera, bool* reset_camera_framebuffer);

GUI gui_create(u32 width, u32 height) {
  GUI gui;

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

  gui.export_image_type = HDR;

  gui.add_type = SPHERE;

  return gui;
}

void gui_update(GUI* gui, Camera* camera, World* world) {
  window_update(gui->window);

  bool reset_camera_framebuffer = false;

  window_imgui_begin_frame();
    gui_update_main_menu_bar(gui);
    if (gui->show_export_warning_window) { gui_update_window_export_warning(gui, camera, world); }
    igDockSpaceOverViewport(igGetID_Str("dockspace"), NULL, ImGuiDockNodeFlags_PassthruCentralNode, NULL);
    if (gui->show_render_window) { gui_update_window_render(gui, camera); }
    if (gui->show_camera_window) { gui_update_window_camera(gui, camera, world, &reset_camera_framebuffer); }
    if (gui->show_world_window) { gui_update_window_world(gui, world, camera, &reset_camera_framebuffer); }
  window_imgui_end_frame();

  if (reset_camera_framebuffer) { camera_clear_framebuffer(camera); }
}

static void gui_update_main_menu_bar(GUI* gui) {
  igBeginMainMenuBar();
    if (igBeginMenu("File", true)) {
      if (igMenuItem_Bool("Export", NULL, false, true)) {
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
    igCombo_Str("Export Type", (s32*) &gui->export_image_type, IMAGE_TYPES_STRING, 0);

    igSeparator();

    igText("Exporting WILL freeze the GUI until the render is complete.");
    igText("Are you sure you want to continue?");
    if (igSmallButton("Yes")) {
      camera_render_export(camera, world);

      switch (gui->export_image_type) {
        case HDR: image_create_hdr("output.hdr", camera); break;
        case JPG: image_create_jpg("output.jpg", camera); break;
      }

      gui->show_export_warning_window = false;
    }

    igSameLine(0, gui->window->imgui_context->Style.ItemInnerSpacing.x);

    if (igSmallButton("No")) { gui->show_export_warning_window = false; }
  igEnd();
}

static void gui_update_window_render(GUI* gui, Camera* camera) {
  if (camera->render || camera->sample_count <= camera->sample_limit) {
    usize framebuffer_length = camera->width * camera->height;
    for (usize i = 0; i < framebuffer_length; i++) {
      gui->framebufferRGB[i] = tonemapping(camera->tonemapping_operator, color_scale(camera->framebuffer[i], 1.0f / camera->sample_count));
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

    igText("FPS: %0.2f", igGetIO_ContextPtr(gui->window->imgui_context)->Framerate);
    igText("Samples: %d", camera->sample_count);

    igSeparatorText("Settings");

    if (igDragFloat3("Position", camera->position.data, 0.1f, -1000.0f, 1000.0f, "%0.2f", 0)) { *reset_camera_framebuffer = true; }
    if (igDragFloat("Focal Length", &camera->focal_length, 0.05f, 0.0f, 1000.0f, "%0.2f", 0)) { *reset_camera_framebuffer = true; }

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
    igCombo_Str("Tonemapping", (s32*) &camera->tonemapping_operator, TONEMAPPING_OPERATORS_STRING, 0);
    switch (camera->tonemapping_operator.type) {
      case CLAMP: break; // clamp doesnt use any variables
      case REINHARD: {
        igDragFloat("Max White", &camera->tonemapping_operator.max_white, 0.1f, 0.0f, 1000.0f, "%0.2f", 0);
      } break;
    }

    if (igSliderInt("Thread Count", (s32*) &camera->thread_count, 1, MAX_THREAD_COUNT, "%u", 0)) {
      camera_render_workers_destroy(camera);
      camera_render_workers_create(camera, world);
    }
    igCheckbox("Render", &camera->render);
    if (igSmallButton("Reset framebuffer")) { *reset_camera_framebuffer = true; }
  igEnd();
}

static void gui_update_window_world(GUI* gui, World* world, Camera* camera, bool* reset_camera_framebuffer) {
  bool remove_hittable = false;
  usize remove_hittable_index;

  igBegin("World", &gui->show_world_window, 0);
    igSeparatorText("Settings");

    if (igCheckbox("Indirect Light Sampling", &world->indirect_light_sampling)) { *reset_camera_framebuffer = true; }
    /*
    if (igCheckbox("Direct Light Sampling (BROKEN)", &world->direct_light_sampling)) { *reset_camera_framebuffer = true; }
    */
    igInputInt("Max Ray Bounces", (s32*) &world->max_ray_bounces, 1, 1, 0);
    if (igColorEdit3("Sky Color", world->sky_color.data, ImGuiColorEditFlags_NoPicker)) { *reset_camera_framebuffer = true; }

    igSeparatorText("Scene");

    if (igSmallButton("Save")) {
      NFD_Init();
      nfdchar_t *save_path;
      nfdfilteritem_t filter_items[1] = { { "Scene file", "scene" } };
      nfdresult_t result = NFD_SaveDialog(&save_path, filter_items, 1, NULL, "untitled.scene");
      if (result == NFD_OKAY) {
        world_scene_save(world, camera, save_path);
        NFD_FreePath(save_path);
      }
      NFD_Quit();
    }

    igSameLine(0, gui->window->imgui_context->Style.ItemInnerSpacing.x);

    if (igSmallButton("Load")) {
      NFD_Init();
      nfdchar_t *out_path;
      nfdfilteritem_t filter_items[1] = { { "Scene file", "scene" } };
      nfdresult_t result = NFD_OpenDialog(&out_path, filter_items, 1, NULL);
      if (result == NFD_OKAY) {
        world_scene_load(world, camera, out_path);
        NFD_FreePath(out_path);
      }
      NFD_Quit();

      *reset_camera_framebuffer = true;
    }

    igSeparator();

    for (usize i = 0; i < world->hittables_count; i++) {
      igPushID_Int(i);

      Hittable* hittable = world->hittables[i];

      if (igCollapsingHeader_BoolPtr(hittable->identifer, NULL, 0)) {
        igSeparatorText("Settings");

        if (igDragFloat3("Position", hittable->position->data, 0.1f, -1000.0f, 1000.0f, "%0.2f", 0)) { *reset_camera_framebuffer = true; }

        switch (hittable->type) {
          case SPHERE: {
            Sphere* sphere = (Sphere*) hittable;

            if (igDragFloat("Radius", &sphere->radius, 0.1f, -1000.0f, 1000.0f, "%0.2f", 0)) { *reset_camera_framebuffer = true; }
          } break;
          case PLANE: {
            Plane* plane = (Plane*) hittable;

            if (igDragFloat3("Normal", plane->normal.data, 0.01f, -1.0f, 1.0f, "%0.2f", 0)) {
              plane->normal = vector3_normalize(plane->normal);
              hittable_plane_update_tangent_vectors(plane);
              *reset_camera_framebuffer = true;
            }
            if (igDragFloat2("Size", plane->size.data, 0.1f, 0.0f, 1000.0f, "%0.2f", 0)) { *reset_camera_framebuffer = true; }
          } break;
        }

        igSeparator();

        if (igSmallButton("Remove")) {
          remove_hittable = true;
          remove_hittable_index = i;
          *reset_camera_framebuffer = true;
        }

        igSeparatorText("Material");
        Material* material = world->hittables[i]->material;

        MaterialType new_type = material->type;
        if (igCombo_Str("Material Type", (s32*) &new_type, MATERIAL_TYPES_STRING, 0)) {
          Color old_albedo;
          switch (material->type) {
            case DIFFUSE: old_albedo = ((Diffuse*) material)->albedo; break;
            case METAL: old_albedo = ((Metal*) material)->albedo; break;
            case GLASS: old_albedo = ((Glass*) material)->albedo; break;
            case EMISSIVE: old_albedo = ((Emissive*) material)->albedo; break;
          }

          material->destroy(world->hittables[i]->material);

          switch (new_type) {
            case DIFFUSE: world->hittables[i]->material = (Material*) material_diffuse_create(old_albedo); break;
            case METAL: world->hittables[i]->material = (Material*) material_metal_create(old_albedo, 0.0f); break;
            case GLASS: world->hittables[i]->material = (Material*) material_glass_create(old_albedo, 1.33f, 0.0f); break;
            case EMISSIVE: world->hittables[i]->material = (Material*) material_emissive_create(old_albedo, 0.5f); break;
          }

          material = world->hittables[i]->material;
          *reset_camera_framebuffer = true;
        }

        igSeparator();

        switch(hittable->material->type) {
          case DIFFUSE: {
            Diffuse* diffuse = (Diffuse*) material;

            if (igColorEdit3("Albedo", diffuse->albedo.data, 0)) { *reset_camera_framebuffer = true; }
          } break;
          case METAL: {
            Metal* metal = (Metal*) material;

            if (igColorEdit3("Albedo", metal->albedo.data, 0)) { *reset_camera_framebuffer = true; }
            if (igDragFloat("Roughness", &metal->roughness, 0.1f, 0.0f, 1.0f, "%0.2f", 0)) { *reset_camera_framebuffer = true; }
          } break;
          case GLASS: {
            Glass* glass = (Glass*) material;

            if (igColorEdit3("Albedo", glass->albedo.data, 0)) { *reset_camera_framebuffer = true; }
            if (igDragFloat("Refraction Index", &glass->refraction_index, 0.1f, 0.0f, 50.0f, "%0.2f", 0)) { *reset_camera_framebuffer = true; }
            if (igDragFloat("Roughness", &glass->roughness, 0.1f, 0.0f, 1.0f, "%0.2f", 0)) { *reset_camera_framebuffer = true; }
          } break;
          case EMISSIVE: {
            Emissive* emissive = (Emissive*) material;

            if (igColorEdit3("Albedo", emissive->albedo.data, 0)) { *reset_camera_framebuffer = true; }
            if (igDragFloat("Emission Strength", &emissive->emission_strength, 0.1f, 0.0f, 1000.0f, "%0.2f", 0)) { *reset_camera_framebuffer = true; }
          } break;
        }
      }
      igPopID();
    }

    igSeparator();

    igCombo_Str("##", (s32*) &gui->add_type, HITTABLE_TYPES_STRING, 0);

    igSameLine(0, gui->window->imgui_context->Style.ItemInnerSpacing.x);

    if (igSmallButton("Add")) {
      Material* default_material = (Material*) material_diffuse_create((Color) { 0.75f, 0.75f, 0.75f });

      Hittable* new_hittable;
      switch (gui->add_type) {
        case SPHERE: new_hittable = (Hittable*) hittable_sphere_create((Vector3) { 0.0f, 0.0f, 0.0f }, 1.0f, default_material); break;
        case PLANE: new_hittable = (Hittable*) hittable_plane_create((Vector3) { 0.0f, 0.0f, 0.0f }, (Vector3) { 0.0f, 1.0f, 0.0f}, (Vector2) { 10.0f, 10.0f }, default_material); break;
      }

      world_add(world, new_hittable);
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
