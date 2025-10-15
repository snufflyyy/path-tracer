#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#include "types/base_types.h"
#include "world.h"
#include "camera.h"
#include "gui/gui.h"

int main() {
  srand(time(NULL));

  World world = world_create();
  Camera camera = camera_create(1280, 720);
  camera.position = (Vector3) { 0.0f, 0.0f, 5.0f };

  world_add(&world, (Sphere) {(Vector3) { 0.0f, 0.0f, 0.0f }, 1.0f});
  world_add(&world, (Sphere) {(Vector3) { 0.0f, -101.0f, 0.0f }, 100.0f});

  GUI gui = gui_create(camera.width, camera.height);
  while (window_is_running(gui.window)) {
    camera_render(&camera, &world);
    gui_update(&gui, &camera, &world);
    gui_render(&gui, camera.framebuffer, camera.sample_count, camera.width, camera.height);
  }
  world_destroy(&world);
  gui_destroy(&gui);
}
