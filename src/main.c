#include "world.h"
#include "camera.h"
#include "gui/gui.h"

#include "textures/image.h"
#include "materials/diffuse.h"
#include "materials/glass.h"
#include "hittables/sphere.h"
#include "hittables/plane.h"

int main() {
  World world = world_create();
  Camera* camera = camera_create(640, 480, &world);
  camera->position.z = 5.0f;

  Texture* texture_brick = (Texture*) texture_image_create("../assets/texture.jpg");
  Texture* texture_earth = (Texture*) texture_image_create("../assets/earth.jpg");
  Material* material_brick = (Material*) material_diffuse_create(texture_brick);
  Material* material_earth = (Material*) material_glass_create(texture_earth, 0.5f, 0.0f);

  world_add(&world, (Hittable*) hittable_sphere_create((Vector3) { 0.0f, 0.0f, 0.0f }, 1.0f, material_earth));
  world_add(&world, (Hittable*) hittable_plane_create((Vector3) { 0.0f, -1.0f, 0.0f }, (Vector3) { 0.0f, 1.0f, 0.0f }, (Vector2) { 5.0f, 5.0f }, material_brick));

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
