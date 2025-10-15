#include "image.h"
#include "math/vector3.h"
#include "world.h"
#include "camera.h"
#include "gui/gui.h"
#include <stdlib.h>
#include <time.h>

int main() {
  srand(time(NULL));

  World world = world_create();
  Camera camera = camera_create(1280, 720);
  camera.position = (Vector3) { 0.0f, 0.0f, -5.0f };

  world_add(&world, (Sphere) {(Vector3) { 0.0f, 0.0f, 0.0f }, 1.0f});
  //world_add(&world, (Sphere) {(Vector3) { 0.0f, -11.0f, 0.0f }, 10.0f});
  
  //GUI gui = gui_create(camera.width, camera.height);
  //while (window_is_running(gui.window)) {
    camera_render(&camera, &world);
  image_create("output.png", camera.framebuffer, camera.width, camera.height);
    //gui_update(&gui, &camera);
    //gui_render(&gui, &camera);
  //}
  world_destroy(&world);
  //gui_destroy(&gui);
}
