#include "math/vector3.h"
#include "types/material.h"
#include "world.h"
#include "camera.h"
#include "hittables/sphere.h"
#include "gui/gui.h"

#define CAMERA_WIDTH 640
#define CAMERA_HEIGHT 480

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

int main() {
  World world = world_create();
  Camera* camera = camera_create(CAMERA_WIDTH, CAMERA_HEIGHT, &world);
  camera->position = (Vector3) { 0.0f, 0.0f, 5.0f };

  world_add(&world, (Hittable*) sphere_create((Vector3) { 0.0f, 0.0f, 0.0f }, 1.0f, MATERIAL_RED));
  world_add(&world, (Hittable*) sphere_create((Vector3) { 0.0f, -101.0f, 0.0f }, 100.0f, MATERIAL_GRAY));

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
