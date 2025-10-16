#include "math/vector3.h"
#include "types/material.h"
#include "world.h"
#include "camera.h"
#include "hittables/sphere.h"
#include "gui/gui.h"

int main() {
  World world = world_create();
  Camera* camera = camera_create(1280, 720, &world);
  camera->position = (Vector3) { 0.0f, 0.0f, 5.0f };

  world_add(&world, (Hittable*) sphere_create((Vector3) { 0.0f, 0.0f, 0.0f }, 1.0f, MATERIAL_RED));
  world_add(&world, (Hittable*) sphere_create((Vector3) { 0.0f, -101.0f, 0.0f }, 100.0f, MATERIAL_GRAY));

  GUI gui = gui_create(camera->width, camera->height);
  while (window_is_running(gui.window)) {
    gui_update(&gui, camera, &world);

    camera_render(camera, &world);
    gui_render(&gui, camera->framebuffer, camera->sample_count, camera->width, camera->height);
  }

  gui_destroy(&gui);
  camera_destroy(camera);
  world_destroy(&world);
}
