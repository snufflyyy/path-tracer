#include "world.h"

#include <stdlib.h>
#include <stdio.h>
#include <cJSON.h>
#include <string.h>

#include "camera.h"
#include "hittables/hittable.h"
#include "hittables/sphere.h"
#include "hittables/plane.h"

#include "materials/material.h"
#include "materials/diffuse.h"
#include "materials/metal.h"
#include "materials/glass.h"
#include "materials/emissive.h"

#include "math/vector3.h"
#include "utils/file.h"

static cJSON* world_json_create_sphere(Sphere* sphere);
static Sphere* world_json_parse_sphere(cJSON* sphere_json);

static cJSON* world_json_create_plane(Plane* plane);
static Plane* world_json_parse_plane(cJSON* plane_json);

static cJSON* world_json_create_material_diffuse(Diffuse* diffuse);
static Diffuse* world_json_parse_material_diffuse(cJSON* diffuse_json);

static cJSON* world_json_create_material_metal(Metal* metal);
static Metal* world_json_parse_material_metal(cJSON* metal_json);

static cJSON* world_json_create_material_glass(Glass* glass);
static Glass* world_json_parse_material_glass(cJSON* glass_json);

static cJSON* world_json_create_material_emissive(Emissive* emissive);
static Emissive* world_json_parse_material_emissive(cJSON* emissive_json);

World world_create() {
  World world = {0};

  world.capacity = WORLD_STARTING_CAPACITY;
  world.hittables = (Hittable**) malloc(sizeof(Hittable*) * world.capacity);
  if (!world.hittables) {
    fprintf(stderr, "[ERROR] [WORLD] Failed to allocate memory for world!\n");
    return (World) {0};
  }

  world.indirect_light_sampling = true;
  world.direct_light_sampling = false;

  world.max_ray_bounces = DEFAULT_MAX_RAY_BOUNCES;
  world.sky_color = DEFAULT_SKY_COLOR;

  return world;
}

void world_add(World* world, Hittable* object) {
  if (world->hittables_count + 1 >= world->capacity) {
    Hittable** temp = (Hittable**) realloc(world->hittables, sizeof(Hittable*) * (world->capacity * WORLD_SCALE_FACTOR));
    if (!temp) {
      fprintf(stderr, "[ERROR] [WORLD] Failed to reallocate memory while increasing world capacity!\n");
      return;
    }

    world->hittables = temp;
    world->capacity *= WORLD_SCALE_FACTOR;
  }

  world->hittables[world->hittables_count] = object;
  world->hittables_count++;
}

void world_remove(World* world, usize index) {
  if (world->hittables_count <= world->capacity / WORLD_SCALE_FACTOR) {
    Hittable** temp = (Hittable**) realloc(world->hittables, sizeof(Hittable*) * (world->capacity / WORLD_SCALE_FACTOR));
    if (!temp) {
      fprintf(stderr, "[ERROR] [WORLD] Failed to reallocate memory while shrinking world capacity!\n");
      return;
    }

    world->hittables = temp;
    world->capacity /= WORLD_SCALE_FACTOR;
  }

  world->hittables[index]->destroy(world->hittables[index]);
  for (usize i = index; i < world->hittables_count - 1; i++) {
    world->hittables[i] = world->hittables[i + 1];
  }
  world->hittables_count--;
}

void world_scene_save(World* world, Camera* camera, const char* filename) {
  cJSON* scene = cJSON_CreateObject();
  if (!scene) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] Failed to create scene JSON object!\n");
    return;
  }

  cJSON* camera_json = cJSON_CreateObject();
  if (!camera_json) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [CAMERA] Failed to create camera JSON object!\n");
    cJSON_Delete(scene);
    return;
  }

  cJSON* camera_position = cJSON_AddArrayToObject(camera_json, "position");
  if (!camera_position) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [CAMERA] Failed to add position JSON array to camera JSON object!\n");
    cJSON_Delete(scene);
    return;
  }

  for (usize i = 0; i < 3; i++) {
    cJSON* element = cJSON_CreateNumber(camera->position.data[i]);
    if (!element) {
      fprintf(stderr, "[ERROR] [WORLD] [JSON] [CAMERA] Failed to create position element JSON number!\n");
      cJSON_Delete(scene);
      return;
    }

    if (!cJSON_AddItemToArray(camera_position, element)) {
      fprintf(stderr, "[ERROR] [WORLD] [JSON] [CAMERA] Failed to add position element JSON number to position JSON array!\n");
      cJSON_Delete(scene);
      return;
    }
  }

  cJSON_AddItemToObject(scene, "camera", camera_json);

  cJSON* hittables = cJSON_AddArrayToObject(scene, "hittables");
  if (!hittables) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] Failed to create hittables JSON array!\n");
    cJSON_Delete(scene);
    return;
  }

  for (usize i = 0; i < world->hittables_count; i++) {
    cJSON* hittable;
    switch (world->hittables[i]->type) {
      case SPHERE: hittable = world_json_create_sphere((Sphere*) world->hittables[i]); break;
      case PLANE: hittable = world_json_create_plane((Plane*) world->hittables[i]); break;
    }

    if (!hittable) {
      fprintf(stderr, "[ERROR] [WORLD] [JSON] Failed to create hittable (index: %lu) JSON object!\n", i);
      cJSON_Delete(scene);
      return;
    }

    cJSON* material;
    switch (world->hittables[i]->material->type) {
      case DIFFUSE: material = world_json_create_material_diffuse((Diffuse*) world->hittables[i]->material); break;
      case METAL: material = world_json_create_material_metal((Metal*) world->hittables[i]->material); break;
      case GLASS: material = world_json_create_material_glass((Glass*) world->hittables[i]->material); break;
      case EMISSIVE: material = world_json_create_material_emissive((Emissive*) world->hittables[i]->material); break;
    }

    if (!material) {
      fprintf(stderr, "[ERROR] [WORLD] [JSON] Failed to create hittable (index: %lu) material JSON object!\n", i);
      cJSON_Delete(scene);
      return;
    }

    cJSON_AddItemToObject(hittable, "material", material);
    cJSON_AddItemToArray(hittables, hittable);
  }

  const char* string = cJSON_Print(scene);
  file_write_string(filename, string);
  free((void*) string);

  cJSON_Delete(scene);
}

void world_scene_load(World* world, Camera* camera, const char* filename) {
  const char* string = file_to_string(filename);
  if (!string) {
    fprintf(stderr, "[ERROR] [WORLD] [SCENE] Failed to open file: %s when loading scene!\n", filename);
    return;
  }

  cJSON* scene = cJSON_Parse(string);
  if (!scene) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] Failed to create scene JSON object!\n");
    free((void*) string);
    return;
  }

  free((void*) string);

  if (world->hittables_count > 0) {
    world_destroy(world);
    *world = world_create();
  }

  cJSON* camera_json = cJSON_GetObjectItemCaseSensitive(scene, "camera");
  if (!camera || !cJSON_IsObject(camera_json)) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] Failed to get camera JSON object!\n");
    cJSON_Delete(scene);
    return;
  }

  cJSON* camera_position = cJSON_GetObjectItemCaseSensitive(camera_json, "position");
  if (!camera_position || !cJSON_IsArray(camera_position)) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] Failed to get camera position JSON array!\n");
    cJSON_Delete(scene);
    return;
  }

  camera->position = (Vector3) { cJSON_GetNumberValue(cJSON_GetArrayItem(camera_position, 0)), cJSON_GetNumberValue(cJSON_GetArrayItem(camera_position, 1)), cJSON_GetNumberValue(cJSON_GetArrayItem(camera_position, 2)) };

  cJSON* hittables = cJSON_GetObjectItemCaseSensitive(scene, "hittables");
  if (!hittables || !cJSON_IsArray(hittables)) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] Failed to get hittables JSON array!\n");
    cJSON_Delete(scene);
    return;
  }

  s32 hittables_count = cJSON_GetArraySize(hittables);
  for (usize i = 0; i < hittables_count; i++) {
    cJSON* hittable = cJSON_GetArrayItem(hittables, i);
    if (!hittable) {
      fprintf(stderr, "[ERROR] [WORLD] [JSON] Failed to get hittable (index: %lu)!\n", i);
      cJSON_Delete(scene);
      return;
    }

    cJSON* type = cJSON_GetObjectItemCaseSensitive(hittable, "type");
    if (!type || !cJSON_IsNumber(type)) {
      fprintf(stderr, "[ERROR] [WORLD] [JSON] Failed to get hittable's type JSON number (enum)!\n");
      cJSON_Delete(scene);
      return;
    }

    Hittable* new_hittable;
    switch ((HittableType) cJSON_GetNumberValue(type)) {
      case SPHERE: new_hittable = (Hittable*) world_json_parse_sphere(hittable); break;
      case PLANE: new_hittable = (Hittable*) world_json_parse_plane(hittable); break;
    }

    if (!new_hittable) {
      fprintf(stderr, "[ERROR] [WORLD] [JSON] Failed to get create new hittable from JSON object!\n");
      cJSON_Delete(scene);
      return;
    }

    cJSON* material_json = cJSON_GetObjectItemCaseSensitive(hittable, "material");
    if (!material_json || !cJSON_IsObject(material_json)) {
      fprintf(stderr, "[ERROR] [WORLD] [JSON] Failed to get material from hittable JSON object!\n");
      cJSON_Delete(scene);
      return;
    }

    cJSON* material_type = cJSON_GetObjectItemCaseSensitive(material_json, "type");
    if (!material_type || !cJSON_IsNumber(material_type)) {
      fprintf(stderr, "[ERROR] [WORLD] [JSON] Failed to get hittable material's type JSON number (enum)!\n");
      cJSON_Delete(scene);
      return;
    }

    Material* material;
    switch ((MaterialType) cJSON_GetNumberValue(material_type)) {
      case DIFFUSE: material = (Material*) world_json_parse_material_diffuse(material_json); break;
      case METAL: material = (Material*) world_json_parse_material_metal(material_json); break;
      case GLASS: material = (Material*) world_json_parse_material_glass(material_json); break;
      case EMISSIVE: material = (Material*) world_json_parse_material_emissive(material_json); break;
    }

    if (!material) {
      fprintf(stderr, "[ERROR] [WORLD] [JSON] Failed to create new hittable's material from JSON object!\n");
      cJSON_Delete(scene);
      return;
    }

    new_hittable->material = material;
    world_add(world, new_hittable);
  }

  cJSON_Delete(scene);
}

static cJSON* world_json_create_sphere(Sphere* sphere) {
  cJSON* hittable = cJSON_CreateObject();
  if (!hittable) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [SPHERE] Failed to create sphere hittable JSON object!\n");
    return NULL;
  }

  if (!cJSON_AddNumberToObject(hittable, "type", SPHERE)) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [SPHERE] Failed to add type JSON number (enum) to sphere JSON object!\n");
    cJSON_Delete(hittable);
    return NULL;
  }

  cJSON* position = cJSON_AddArrayToObject(hittable, "position");
  if (!position) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [SPHERE] Failed to add position JSON array to sphere JSON object!\n");
    cJSON_Delete(hittable);
    return NULL;
  }

  for (usize i = 0; i < 3; i++) {
    cJSON* element = cJSON_CreateNumber(sphere->position.data[i]);
    if (!element) {
      fprintf(stderr, "[ERROR] [WORLD] [JSON] [SPHERE] Failed to create position element JSON number!\n");
      cJSON_Delete(hittable);
      return NULL;
    }

    if (!cJSON_AddItemToArray(position, element)) {
      fprintf(stderr, "[ERROR] [WORLD] [JSON] [SPHERE] Failed to add position element JSON number to position JSON array!\n");
      cJSON_Delete(hittable);
      return NULL;
    }
  }

  if (!cJSON_AddNumberToObject(hittable, "radius", sphere->radius)) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [SPHERE] Failed to add radius JSON number to sphere JSON object!\n");
    cJSON_Delete(hittable);
    return NULL;
  }

  return hittable;
}

static Sphere* world_json_parse_sphere(cJSON* sphere_json) {
  cJSON* position = cJSON_GetObjectItemCaseSensitive(sphere_json, "position");
  if (!position || !cJSON_IsArray(position)) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [SPHERE] Failed to parse position JSON array from sphere JSON object!\n");
    return NULL;
  }

  cJSON* radius = cJSON_GetObjectItemCaseSensitive(sphere_json, "radius");
  if (!radius || !cJSON_IsNumber(radius)) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [SPHERE] Failed to parse radius JSON number from sphere JSON object!\n");
    return NULL;
  }

  Vector3 new_position = { cJSON_GetNumberValue(cJSON_GetArrayItem(position, 0)), cJSON_GetNumberValue(cJSON_GetArrayItem(position, 1)), cJSON_GetNumberValue(cJSON_GetArrayItem(position, 2)) };
  return hittable_sphere_create(new_position, cJSON_GetNumberValue(radius), NULL);
}

static cJSON* world_json_create_plane(Plane* plane) {
  cJSON* hittable = cJSON_CreateObject();
  if (!hittable) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [PLANE] Failed to create plane hittable JSON object!\n");
    return NULL;
  }

  if (!cJSON_AddNumberToObject(hittable, "type", PLANE)) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [PLANE] Failed to add type JSON number (enum) to plane JSON object!\n");
    cJSON_Delete(hittable);
    return NULL;
  }

  cJSON* position = cJSON_AddArrayToObject(hittable, "position");
  if (!position) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [PLANE] Failed to add position JSON array to plane JSON object!\n");
    cJSON_Delete(hittable);
    return NULL;
  }

  for (usize i = 0; i < 3; i++) {
    cJSON* element = cJSON_CreateNumber(plane->position.data[i]);
    if (!element) {
      fprintf(stderr, "[ERROR] [WORLD] [JSON] [PLANE] Failed to create position element JSON number!\n");
      cJSON_Delete(hittable);
      return NULL;
    }

    if (!cJSON_AddItemToArray(position, element)) {
      fprintf(stderr, "[ERROR] [WORLD] [JSON] [PLANE] Failed to add position element JSON number to position JSON array!\n");
      cJSON_Delete(hittable);
      return NULL;
    }
  }

  cJSON* normal = cJSON_AddArrayToObject(hittable, "normal");
  if (!normal) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [PLANE] Failed to add normal JSON array to plane JSON object!\n");
    cJSON_Delete(hittable);
    return NULL;
  }

  for (usize i = 0; i < 3; i++) {
    cJSON* element = cJSON_CreateNumber(plane->normal.data[i]);
    if (!element) {
      fprintf(stderr, "[ERROR] [WORLD] [JSON] [PLANE] Failed to create normal element JSON number!\n");
      cJSON_Delete(hittable);
      return NULL;
    }

    if (!cJSON_AddItemToArray(normal, element)) {
      fprintf(stderr, "[ERROR] [WORLD] [JSON] [PLANE] Failed to add normal element JSON number to normal JSON array!\n");
      cJSON_Delete(hittable);
      return NULL;
    }
  }

  cJSON* size = cJSON_AddArrayToObject(hittable, "size");
  if (!size) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [PLANE] Failed to add size JSON array to plane JSON object!\n");
    cJSON_Delete(hittable);
    return NULL;
  }

  for (usize i = 0; i < 2; i++) {
    cJSON* element = cJSON_CreateNumber(plane->size.data[i]);
    if (!element) {
      fprintf(stderr, "[ERROR] [WORLD] [JSON] [PLANE] Failed to create size element JSON number!\n");
      cJSON_Delete(hittable);
      return NULL;
    }

    if (!cJSON_AddItemToArray(size, element)) {
      fprintf(stderr, "[ERROR] [WORLD] [JSON] [PLANE] Failed to add size element JSON number to size JSON array!\n");
      cJSON_Delete(hittable);
      return NULL;
    }
  }

  return hittable;
}

static Plane* world_json_parse_plane(cJSON* plane_json) {
  cJSON* position = cJSON_GetObjectItemCaseSensitive(plane_json, "position");
  if (!position || !cJSON_IsArray(position)) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [PLANE] Failed to parse position JSON array from plane JSON object!\n");
    return NULL;
  }

  cJSON* normal = cJSON_GetObjectItemCaseSensitive(plane_json, "normal");
  if (!normal || !cJSON_IsArray(normal)) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [PLANE] Failed to parse normal JSON array from plane JSON object!\n");
    return NULL;
  }

  cJSON* size = cJSON_GetObjectItemCaseSensitive(plane_json, "size");
  if (!size || !cJSON_IsArray(size)) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [PLANE] Failed to parse size JSON array from plane JSON object!\n");
    return NULL;
  }

  Vector3 new_position = { cJSON_GetNumberValue(cJSON_GetArrayItem(position, 0)), cJSON_GetNumberValue(cJSON_GetArrayItem(position, 1)), cJSON_GetNumberValue(cJSON_GetArrayItem(position, 2)) };
  Vector3 new_normal = { cJSON_GetNumberValue(cJSON_GetArrayItem(normal, 0)), cJSON_GetNumberValue(cJSON_GetArrayItem(normal, 1)), cJSON_GetNumberValue(cJSON_GetArrayItem(normal, 2)) };
  Vector2 new_size = { cJSON_GetNumberValue(cJSON_GetArrayItem(size, 0)), cJSON_GetNumberValue(cJSON_GetArrayItem(size, 1)) };
  return hittable_plane_create(new_position, new_normal, new_size, NULL);
}

static cJSON* world_json_create_material_diffuse(Diffuse* diffuse) {
  cJSON* material = cJSON_CreateObject();
  if (!material) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [MATERIAL] [DIFFUSE] Failed to create diffuse material JSON object!");
    return NULL;
  }

  if (!cJSON_AddNumberToObject(material, "type", DIFFUSE)) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [MATERIAL] [DIFFUSE] Failed to add type JSON number (enum) to diffuse material JSON object!\n");
    cJSON_Delete(material);
    return NULL;
  }

  cJSON* albedo = cJSON_AddArrayToObject(material, "albedo");
  if (!albedo) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [MATERIAL] [DIFFUSE] Failed to add albedo JSON array to diffuse material JSON object!\n");
    cJSON_Delete(material);
    return NULL;
  }

  for (usize i = 0; i < 3; i++) {
    cJSON* element = cJSON_CreateNumber(diffuse->albedo.data[i]);
    if (!element) {
      fprintf(stderr, "[ERROR] [WORLD] [JSON] [MATERIAL] [DIFFUSE] Failed to create albedo element JSON number!\n");
      cJSON_Delete(material);
      return NULL;
    }

    if (!cJSON_AddItemToArray(albedo, element)) {
      fprintf(stderr, "[ERROR] [WORLD] [JSON] [MATERIAL] [DIFFUSE] Failed to add albedo element JSON number to albedo JSON array!\n");
      cJSON_Delete(material);
      return NULL;
    }
  }

  return material;
}

static Diffuse* world_json_parse_material_diffuse(cJSON* diffuse_json) {
  cJSON* albedo = cJSON_GetObjectItemCaseSensitive(diffuse_json, "albedo");
  if (!albedo) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [MATERIAL] [DIFFUSE] Failed to get albedo JSON array!\n");
    return NULL;
  }

  return material_diffuse_create((Color) { cJSON_GetNumberValue(cJSON_GetArrayItem(albedo, 0)), cJSON_GetNumberValue(cJSON_GetArrayItem(albedo, 1)), cJSON_GetNumberValue(cJSON_GetArrayItem(albedo, 2)) });
}

static cJSON* world_json_create_material_metal(Metal* metal) {
  cJSON* material = cJSON_CreateObject();
  if (!material) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [MATERIAL] [METAL] Failed to create metal material JSON object!");
    return NULL;
  }

  if (!cJSON_AddNumberToObject(material, "type", METAL)) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [MATERIAL] [METAL] Failed to add type JSON number (enum) to metal material JSON object!\n");
    cJSON_Delete(material);
    return NULL;
  }

  cJSON* albedo = cJSON_AddArrayToObject(material, "albedo");
  if (!albedo) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [MATERIAL] [METAL] Failed to add albedo JSON array to metal material JSON object!\n");
    cJSON_Delete(material);
    return NULL;
  }

  for (usize i = 0; i < 3; i++) {
    cJSON* element = cJSON_CreateNumber(metal->albedo.data[i]);
    if (!element) {
      fprintf(stderr, "[ERROR] [WORLD] [JSON] [MATERIAL] [METAL] Failed to create albedo element JSON number!\n");
      cJSON_Delete(material);
      return NULL;
    }

    if (!cJSON_AddItemToArray(albedo, element)) {
      fprintf(stderr, "[ERROR] [WORLD] [JSON] [MATERIAL] [METAL] Failed to add albedo element JSON number to albedo JSON array!\n");
      cJSON_Delete(material);
      return NULL;
    }
  }

  if (!cJSON_AddNumberToObject(material, "roughness", metal->roughness)) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [MATERIAL] [METAL] Failed to add roughness JSON number to metal material JSON object!\n");
    cJSON_Delete(material);
    return NULL;
  }

  return material;
}

static Metal* world_json_parse_material_metal(cJSON* metal_json) {
  cJSON* albedo = cJSON_GetObjectItemCaseSensitive(metal_json, "albedo");
  if (!albedo) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [MATERIAL] [METAL] Failed to get albedo JSON array!\n");
    return NULL;
  }

  cJSON* roughness = cJSON_GetObjectItemCaseSensitive(metal_json, "roughness");
  if (!roughness) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [MATERIAL] [METAL] Failed to get roughness JSON number!\n");
    return NULL;
  }

  return material_metal_create((Color) { cJSON_GetNumberValue(cJSON_GetArrayItem(albedo, 0)), cJSON_GetNumberValue(cJSON_GetArrayItem(albedo, 1)), cJSON_GetNumberValue(cJSON_GetArrayItem(albedo, 2)) }, cJSON_GetNumberValue(roughness));
}

static cJSON* world_json_create_material_glass(Glass* glass) {
  cJSON* material = cJSON_CreateObject();
  if (!material) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [MATERIAL] [GLASS] Failed to create glass material JSON object!");
    return NULL;
  }

  if (!cJSON_AddNumberToObject(material, "type", GLASS)) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [MATERIAL] [GLASS] Failed to add type JSON number (enum) to glass material JSON object!\n");
    cJSON_Delete(material);
    return NULL;
  }

  cJSON* albedo = cJSON_AddArrayToObject(material, "albedo");
  if (!albedo) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [MATERIAL] [GLASS] Failed to add albedo JSON array to glass material JSON object!\n");
    cJSON_Delete(material);
    return NULL;
  }

  for (usize i = 0; i < 3; i++) {
    cJSON* element = cJSON_CreateNumber(glass->albedo.data[i]);
    if (!element) {
      fprintf(stderr, "[ERROR] [WORLD] [JSON] [MATERIAL] [GLASS] Failed to create albedo element JSON number!\n");
      cJSON_Delete(material);
      return NULL;
    }

    if (!cJSON_AddItemToArray(albedo, element)) {
      fprintf(stderr, "[ERROR] [WORLD] [JSON] [MATERIAL] [GLASS] Failed to add albedo element JSON number to albedo JSON array!\n");
      cJSON_Delete(material);
      return NULL;
    }
  }

  if (!cJSON_AddNumberToObject(material, "roughness", glass->roughness)) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [MATERIAL] [GLASS] Failed to add roughness JSON number to glass material JSON object!\n");
    cJSON_Delete(material);
    return NULL;
  }

  if (!cJSON_AddNumberToObject(material, "refraction-index", glass->refraction_index)) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [MATERIAL] [GLASS] Failed to add refraction index JSON number to glass material JSON object!\n");
    cJSON_Delete(material);
    return NULL;
  }

  return material;
}

static Glass* world_json_parse_material_glass(cJSON* glass_json) {
  cJSON* albedo = cJSON_GetObjectItemCaseSensitive(glass_json, "albedo");
  if (!albedo) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [MATERIAL] [METAL] Failed to get albedo JSON array!\n");
    return NULL;
  }

  cJSON* roughness = cJSON_GetObjectItemCaseSensitive(glass_json, "roughness");
  if (!roughness) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [MATERIAL] [METAL] Failed to get roughness JSON number!\n");
    return NULL;
  }

  cJSON* refraction_index = cJSON_GetObjectItemCaseSensitive(glass_json, "refraction-index");
  if (!roughness) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [MATERIAL] [GLASS] Failed to get refraction index JSON number!\n");
    return NULL;
  }

  return material_glass_create((Color) { cJSON_GetNumberValue(cJSON_GetArrayItem(albedo, 0)), cJSON_GetNumberValue(cJSON_GetArrayItem(albedo, 1)), cJSON_GetNumberValue(cJSON_GetArrayItem(albedo, 2)) }, cJSON_GetNumberValue(refraction_index), cJSON_GetNumberValue(roughness));
}

static cJSON* world_json_create_material_emissive(Emissive* emissive) {
  cJSON* material = cJSON_CreateObject();
  if (!material) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [MATERIAL] [EMISSIVE] Failed to create emissive material JSON object!");
    return NULL;
  }

  if (!cJSON_AddNumberToObject(material, "type", EMISSIVE)) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [MATERIAL] [EMISSIVE] Failed to add type JSON number (enum) to emissive material JSON object!\n");
    cJSON_Delete(material);
    return NULL;
  }

  cJSON* albedo = cJSON_AddArrayToObject(material, "albedo");
  if (!albedo) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [MATERIAL] [EMISSIVE] Failed to add albedo JSON array to emissive material JSON object!\n");
    cJSON_Delete(material);
    return NULL;
  }

  for (usize i = 0; i < 3; i++) {
    cJSON* element = cJSON_CreateNumber(emissive->albedo.data[i]);
    if (!element) {
      fprintf(stderr, "[ERROR] [WORLD] [JSON] [MATERIAL] [EMISSIVE] Failed to create albedo element JSON number!\n");
      cJSON_Delete(material);
      return NULL;
    }

    if (!cJSON_AddItemToArray(albedo, element)) {
      fprintf(stderr, "[ERROR] [WORLD] [JSON] [MATERIAL] [EMISSIVE] Failed to add albedo element JSON number to albedo JSON array!\n");
      cJSON_Delete(material);
      return NULL;
    }
  }

  if (!cJSON_AddNumberToObject(material, "emission-strength", emissive->emission_strength)) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [MATERIAL] [EMISSIVE] Failed to add emission strength JSON number to emissive material JSON object!\n");
    cJSON_Delete(material);
    return NULL;
  }

  return material;
}

static Emissive* world_json_parse_material_emissive(cJSON* emissive_json) {
  cJSON* albedo = cJSON_GetObjectItemCaseSensitive(emissive_json, "albedo");
  if (!albedo) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [MATERIAL] [EMISSIVE] Failed to get albedo JSON array!\n");
    return NULL;
  }

  cJSON* emission_strength = cJSON_GetObjectItemCaseSensitive(emissive_json, "emission-strength");
  if (!emission_strength) {
    fprintf(stderr, "[ERROR] [WORLD] [JSON] [MATERIAL] [METAL] Failed to get roughness JSON number!\n");
    return NULL;
  }

  return material_emissive_create((Color) { cJSON_GetNumberValue(cJSON_GetArrayItem(albedo, 0)), cJSON_GetNumberValue(cJSON_GetArrayItem(albedo, 1)), cJSON_GetNumberValue(cJSON_GetArrayItem(albedo, 2)) }, cJSON_GetNumberValue(emission_strength));
}

void world_destroy(World* world) {
  for (usize i = 0; i < world->hittables_count; i++) {
    world->hittables[i]->destroy(world->hittables[i]);
  }

  free(world->hittables);
  world->hittables = NULL;
}
