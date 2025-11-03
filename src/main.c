#include "world.h"
#include "camera.h"
#include "gui/gui.h"

int main() {
  World world = world_create();
  Camera* camera = camera_create(640, 480, &world);

  world_scene_load(&world, camera, "../scenes/example.scene");

  GUI gui = gui_create(1280, 720);
  while (window_is_running(gui.window)) {
    gui_update(&gui, camera, &world);

    camera_render_frame(camera, &world);
    gui_render(&gui);
  }

  gui_destroy(&gui);
  camera_destroy(camera);
  world_destroy(&world);
}
