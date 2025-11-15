#include "gui/gui.h"
#include "gui/texture.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <cimgui_impl.h>

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
#include "textures/image.h"
#include "textures/solid_color.h"
#include "textures/texture.h"
#include "textures/image.h"
#include "tonemapping.h"
#include "utils/file.h"
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

  gui.add_type = HITTABLE_TYPE_SPHERE;

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
      nfdfilteritem_t filter_items[1];
      switch (gui->export_image_type) {
        case HDR: filter_items[0] = (nfdfilteritem_t) { "HDR", "hdr" }; break;
        case JPG: filter_items[0] = (nfdfilteritem_t) { "JPG", "jpg" }; break;
      }

      const char* path = file_dialog_get_save(filter_items, 1);

      camera_render_export(camera, world);

      switch (gui->export_image_type) {
        case HDR: image_create_hdr(path, camera); break;
        case JPG: image_create_jpg(path, camera); break;
      }

      file_dialog_string_destroy(path);
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

    texture_bind(gui->texture);
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
    if (igCheckbox("Direct Light Sampling (Experimental)", &world->direct_light_sampling)) { *reset_camera_framebuffer = true; }
    igInputInt("Max Ray Bounces", (s32*) &world->max_ray_bounces, 1, 1, 0);
    if (igColorEdit3("Sky Color", world->sky_color.data, ImGuiColorEditFlags_NoPicker)) { *reset_camera_framebuffer = true; }

    igSeparatorText("Scene");

    if (igSmallButton("Save")) {
      NFD_Init();
      nfdfilteritem_t filter_items[] = { { "Scene file", "scene" } };
      const char* path = file_dialog_get_save(filter_items, (sizeof(filter_items) / sizeof(nfdfilteritem_t)));

      world_scene_save(world, camera, path);
      file_dialog_string_destroy(path);
    }

    igSameLine(0, gui->window->imgui_context->Style.ItemInnerSpacing.x);

    if (igSmallButton("Load")) {
      nfdfilteritem_t filter_items[] = { { "Scene file", "scene" } };
      const char* path = file_dialog_get_open(filter_items, (sizeof(filter_items) / sizeof(nfdfilteritem_t)));
      world_scene_load(world, camera, path);
      file_dialog_string_destroy(path);

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
          case HITTABLE_TYPE_SPHERE: {
            HittableSphere* sphere = (HittableSphere*) hittable;

            if (igDragFloat("Radius", &sphere->radius, 0.1f, -1000.0f, 1000.0f, "%0.2f", 0)) { *reset_camera_framebuffer = true; }
          } break;
          case HITTABLE_TYPE_PLANE: {
            HittablePlane* plane = (HittablePlane*) hittable;

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
          Texture* old_albedo;
          switch (material->type) {
            case MATERIAL_TYPE_DIFFUSE: old_albedo = ((MaterialDiffuse*) material)->albedo; break;
            case MATERIAL_TYPE_METAL: old_albedo = ((Metal*) material)->albedo; break;
            case MATERIAL_TYPE_GLASS: old_albedo = ((MaterialGlass*) material)->albedo; break;
            case MATERIAL_TYPE_EMISSIVE: old_albedo = ((MaterialEmissive*) material)->albedo; break;
          }

          material->destroy(world->hittables[i]->material);

          switch (new_type) {
            case MATERIAL_TYPE_DIFFUSE: world->hittables[i]->material = (Material*) material_diffuse_create(old_albedo); break;
            case MATERIAL_TYPE_METAL: world->hittables[i]->material = (Material*) material_metal_create(old_albedo, 0.0f); break;
            case MATERIAL_TYPE_GLASS: world->hittables[i]->material = (Material*) material_glass_create(old_albedo, 1.33f, 0.0f); break;
            case MATERIAL_TYPE_EMISSIVE: world->hittables[i]->material = (Material*) material_emissive_create(old_albedo, 0.5f); break;
          }

          material = world->hittables[i]->material;
          *reset_camera_framebuffer = true;
        }

        igSeparator();

        switch(hittable->material->type) {
          case MATERIAL_TYPE_DIFFUSE: {
            MaterialDiffuse* diffuse = (MaterialDiffuse*) material;

            bool changed = false;
            switch (diffuse->albedo->type) {
              case TEXTURE_TYPE_SOLID_COLOR: changed = texture_solid_color_gui_edit((TextureSolidColor*) diffuse->albedo); break;
              case TEXTURE_TYPE_IMAGE: changed = texture_image_gui_edit((TextureImage*) diffuse->albedo); break;
            }
            if (changed) { *reset_camera_framebuffer = true; }
          } break;
          case MATERIAL_TYPE_METAL: {
            Metal* metal = (Metal*) material;

            bool changed = false;
            switch (metal->albedo->type) {
              case TEXTURE_TYPE_SOLID_COLOR: changed = texture_solid_color_gui_edit((TextureSolidColor*) metal->albedo); break;
              case TEXTURE_TYPE_IMAGE: changed = texture_image_gui_edit((TextureImage*) metal->albedo); break;
            }
            if (changed) { *reset_camera_framebuffer = true; }

            if (igDragFloat("Roughness", &metal->roughness, 0.1f, 0.0f, 1.0f, "%0.2f", 0)) { *reset_camera_framebuffer = true; }
          } break;
          case MATERIAL_TYPE_GLASS: {
            MaterialGlass* glass = (MaterialGlass*) material;

            bool changed = false;
            switch (glass->albedo->type) {
              case TEXTURE_TYPE_SOLID_COLOR: *reset_camera_framebuffer = texture_solid_color_gui_edit((TextureSolidColor*) glass->albedo); break;
              case TEXTURE_TYPE_IMAGE: changed = texture_image_gui_edit((TextureImage*) glass->albedo); break;
            }
            if (changed) { *reset_camera_framebuffer = true; }

            if (igDragFloat("Refraction Index", &glass->refraction_index, 0.1f, 0.0f, 50.0f, "%0.2f", 0)) { *reset_camera_framebuffer = true; }
            if (igDragFloat("Roughness", &glass->roughness, 0.1f, 0.0f, 1.0f, "%0.2f", 0)) { *reset_camera_framebuffer = true; }
          } break;
          case MATERIAL_TYPE_EMISSIVE: {
            MaterialEmissive* emissive = (MaterialEmissive*) material;

            bool changed = false;
            switch (emissive->albedo->type) {
              case TEXTURE_TYPE_SOLID_COLOR: *reset_camera_framebuffer = texture_solid_color_gui_edit((TextureSolidColor*) emissive->albedo); break;
              case TEXTURE_TYPE_IMAGE: changed = texture_image_gui_edit((TextureImage*) emissive->albedo); break;
            }
            if (changed) { *reset_camera_framebuffer = true; }

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
      Material* default_material = (Material*) material_diffuse_create((Texture*) texture_solid_color_create((Color) { 0.75f, 0.75f, 0.75f }));

      Hittable* new_hittable;
      switch (gui->add_type) {
        case HITTABLE_TYPE_SPHERE: new_hittable = (Hittable*) hittable_sphere_create((Vector3) { 0.0f, 0.0f, 0.0f }, 1.0f, default_material); break;
        case HITTABLE_TYPE_PLANE: new_hittable = (Hittable*) hittable_plane_create((Vector3) { 0.0f, 0.0f, 0.0f }, (Vector3) { 0.0f, 1.0f, 0.0f}, (Vector2) { 10.0f, 10.0f }, default_material); break;
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
