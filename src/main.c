#include "world.h"
#include "camera.h"
#include "materials/material.h"
#include "materials/emissive.h"
#include "materials/diffuse.h"
#include "hittables/hittable.h"
#include "hittables/plane.h"
#include "gui/gui.h"

#define CAMERA_WIDTH 640
#define CAMERA_HEIGHT 480

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

int main() {
  World world = world_create();
  Camera* camera = camera_create(CAMERA_WIDTH, CAMERA_HEIGHT, &world);
  camera->position = (Vector3) { 0.0f, 5.0f, 10.0f };

  Diffuse* white = diffuse_material_create((Color) { 1.0f, 1.0f, 1.0f });
  Diffuse* red = diffuse_material_create((Color) { 0.8f, 0.1f, 0.1f });
  Diffuse* green = diffuse_material_create((Color) { 0.1f, 0.8f, 0.1f });
  Emissive* light = emissive_material_create((Color) { 1.0f, 1.0f, 1.0f }, 10.0f);

  world_add(&world, (Hittable*) plane_create((Vector3) { 0.0f, 0.0f, 0.0f }, (Vector3) { 0.0f, 1.0f, 0.0f }, (Vector2) { 10.0f, 10.0f }, (Material*) white)); // ground
  world_add(&world, (Hittable*) plane_create((Vector3) { -5.0f, 5.0f, 0.0f }, (Vector3) { 1.0f, 0.0f, 0.0f }, (Vector2) { 10.0f, 10.0f }, (Material*) red)); // left
  world_add(&world, (Hittable*) plane_create((Vector3) { 5.0f, 5.0f, 0.0f }, (Vector3) { -1.0f, 0.0f, 0.0f }, (Vector2) { 10.0f, 10.0f }, (Material*) green)); // right
  world_add(&world, (Hittable*) plane_create((Vector3) { 0.0f, 5.0f, -5.0f }, (Vector3) { 0.0f, 0.0f, 1.0f }, (Vector2) { 10.0f, 10.0f }, (Material*) white)); // front
  world_add(&world, (Hittable*) plane_create((Vector3) { 0.0f, 5.0f, 5.0f }, (Vector3) { 0.0f, 0.0f, -1.0f }, (Vector2) { 10.0f, 10.0f }, (Material*) white)); // back
  world_add(&world, (Hittable*) plane_create((Vector3) { 0.0f, 10.0f, 0.0f }, (Vector3) { 0.0f, -1.0f, 0.0f }, (Vector2) { 10.0f, 10.0f }, (Material*) white)); // ceiling
  world_add(&world, (Hittable*) plane_create((Vector3) { 0.0, 9.99f, 0.0f }, (Vector3) { 0.0f, -1.0f, 0.0f }, (Vector2) { 5.0f, 2.0f }, (Material*) light)); // light

  GUI gui = gui_create(WINDOW_WIDTH, WINDOW_HEIGHT);
  while (window_is_running(gui.window)) {
    gui_update(&gui, camera, &world);

    camera_render_frame(camera, &world);
    gui_render(&gui);
  }

  gui_destroy(&gui);
  camera_destroy(camera);
  world_destroy(&world);
}
