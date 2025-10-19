#include "hittables/hittable.h"
#include "math/vector3.h"
#include "materials/material.h"
#include "materials/diffuse.h"
#include "materials/glass.h"
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

  // hittables will destroy materials when the hittlable's destroy function is run
  Glass* glass_white = glass_material_create((Color) { 1.0f, 1.0f, 1.0f }, 1.0f / 1.33f, 0.0f );
  Diffuse* diffuse_gray = diffuse_material_create((Color) { 0.75f, 0.75f, 0.75f });

  world_add(&world, (Hittable*) sphere_create((Vector3) { 0.0f, 0.0f, 0.0f }, 1.0f, (Material*) glass_white));
  world_add(&world, (Hittable*) sphere_create((Vector3) { 0.0f, -101.0f, 0.0f }, 100.0f, (Material*) diffuse_gray));

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
