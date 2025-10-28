#include "math/vector3.h"
#include "world.h"
#include "camera.h"
#include "materials/material.h"
#include "materials/diffuse.h"
#include "hittables/hittable.h"
#include "hittables/sphere.h"
#include "hittables/plane.h"
#include "gui/gui.h"

int main() {
  World world = world_create();
  Camera* camera = camera_create(640, 480, &world);
  camera->position = (Vector3) { 0.0f, 0.0f, 5.0f };

  Material* gray = (Material*) material_diffuse_create((Color) { 0.75f, 0.75f, 0.75f });

  world_add(&world, (Hittable*) hittable_sphere_create((Vector3) { 0.0f, 0.0f, 0.0f }, 1.0f, gray));
  world_add(&world, (Hittable*) hittable_plane_create((Vector3) { 0.0f, -1.0f, 0.0f }, (Vector3) { 0.0f, 1.0f, 0.0f }, (Vector2) { 10.0f, 10.0f }, gray));

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
