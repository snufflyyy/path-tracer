#include "camera.h"
#include "gui/gui.h"

int main() {
  Camera camera = camera_create(1280, 720);

  GUI gui = gui_create(camera.width, camera.height);
  while (window_is_running(gui.window)) {
    camera_render(&camera);

    gui_update(&gui, &camera);
    gui_render(&gui, &camera);
  }
  gui_destroy(&gui);
}
