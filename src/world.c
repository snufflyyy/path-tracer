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
  cJSON* scene_json = cJSON_CreateObject();
  if (!scene_json) { goto error; }

  cJSON* camera_json = cJSON_CreateObject();
  if (!camera_json) { goto error; }

  cJSON* camera_position_json = cJSON_AddArrayToObject(camera_json, "position");
  if (!camera_position_json) { goto error; }

  for (usize i = 0; i < 3; i++) {
    cJSON* element = cJSON_CreateNumber(camera->position.data[i]);
    if (!element) { goto error; }

    if (!cJSON_AddItemToArray(camera_position_json, element)) { goto error; }
  }

  cJSON_AddItemToObject(scene_json, "camera", camera_json);

  cJSON* hittables_json = cJSON_AddArrayToObject(scene_json, "hittables");
  if (!hittables_json) { goto error; }

  for (usize i = 0; i < world->hittables_count; i++) {
    cJSON* hittable_json;
    switch (world->hittables[i]->type) {
      case HITTABLE_TYPE_SPHERE: hittable_json = hittable_sphere_json_create((HittableSphere*) world->hittables[i]); break;
      case HITTABLE_TYPE_PLANE: hittable_json = hittable_plane_json_create((HittablePlane*) world->hittables[i]); break;
    }

    if (!hittable_json) { goto error; }

    cJSON* material_json;
    switch (world->hittables[i]->material->type) {
      case MATERIAL_TYPE_DIFFUSE: material_json = material_diffuse_json_create((MaterialDiffuse*) world->hittables[i]->material); break;
      case MATERIAL_TYPE_METAL: material_json = material_metal_json_create((Metal*) world->hittables[i]->material); break;
      case MATERIAL_TYPE_GLASS: material_json = material_glass_json_create((MaterialGlass*) world->hittables[i]->material); break;
      case MATERIAL_TYPE_EMISSIVE: material_json = material_emissive_json_create((MaterialEmissive*) world->hittables[i]->material); break;
    }

    if (!material_json) { goto error; }

    cJSON_AddItemToObject(hittable_json, "material", material_json);
    cJSON_AddItemToArray(hittables_json, hittable_json);
  }

  const char* string = cJSON_Print(scene_json);
  file_write_string(filename, string);
  free((void*) string);

  cJSON_Delete(scene_json);

  return;

error:
  fprintf(stderr, "[ERROR] [WORLD] [SCENE] Failed to save scene!\n");
  //cJSON_Delete(scene_json);
}

void world_scene_load(World* world, Camera* camera, const char* filename) {
  const char* string = file_to_string(filename);
  if (!string) {
    fprintf(stderr, "[ERROR] [WORLD] [SCENE] Failed to load file: %s!\n", filename);
    return;
  }

  cJSON* scene_json = cJSON_Parse(string);
  if (!scene_json) {
    free((void*) string);
    goto error;
  }

  free((void*) string);

  if (world->hittables_count > 0) {
    world_destroy(world);
    *world = world_create();
  }

  cJSON* camera_json = cJSON_GetObjectItemCaseSensitive(scene_json, "camera");
  if (!camera || !cJSON_IsObject(camera_json)) { goto error; }

  cJSON* camera_position = cJSON_GetObjectItemCaseSensitive(camera_json, "position");
  if (!camera_position || !cJSON_IsArray(camera_position)) { goto error; }

  camera->position = (Vector3) { cJSON_GetNumberValue(cJSON_GetArrayItem(camera_position, 0)), cJSON_GetNumberValue(cJSON_GetArrayItem(camera_position, 1)), cJSON_GetNumberValue(cJSON_GetArrayItem(camera_position, 2)) };

  cJSON* hittables_json = cJSON_GetObjectItemCaseSensitive(scene_json, "hittables");
  if (!hittables_json || !cJSON_IsArray(hittables_json)) { goto error; }

  s32 hittables_count = cJSON_GetArraySize(hittables_json);
  for (usize i = 0; i < hittables_count; i++) {
    cJSON* hittable_json = cJSON_GetArrayItem(hittables_json, i);
    if (!hittable_json) { goto error; }

    cJSON* type_json = cJSON_GetObjectItemCaseSensitive(hittable_json, "type");
    if (!type_json || !cJSON_IsNumber(type_json)) { goto error; }

    Hittable* new_hittable;
    switch ((HittableType) cJSON_GetNumberValue(type_json)) {
      case HITTABLE_TYPE_SPHERE: new_hittable = (Hittable*) hittable_sphere_json_parse(hittable_json); break;
      case HITTABLE_TYPE_PLANE: new_hittable = (Hittable*) hittable_plane_json_parse(hittable_json); break;
    }

    if (!new_hittable) { goto error; }

    cJSON* material_json = cJSON_GetObjectItemCaseSensitive(hittable_json, "material");
    if (!material_json || !cJSON_IsObject(material_json)) { goto error; }

    cJSON* material_type_json = cJSON_GetObjectItemCaseSensitive(material_json, "type");
    if (!material_type_json || !cJSON_IsNumber(material_type_json)) { goto error; }

    Material* material;
    switch ((MaterialType) cJSON_GetNumberValue(material_type_json)) {
      case MATERIAL_TYPE_DIFFUSE: material = (Material*) material_diffuse_json_parse(material_json); break;
      case MATERIAL_TYPE_METAL: material = (Material*) material_metal_json_parse(material_json); break;
      case MATERIAL_TYPE_GLASS: material = (Material*) material_glass_json_parse(material_json); break;
      case MATERIAL_TYPE_EMISSIVE: material = (Material*) material_emissive_json_parse(material_json); break;
    }

    if (!material) { goto error; }

    new_hittable->material = material;
    world_add(world, new_hittable);
  }

  cJSON_Delete(scene_json);

  return;

error:
  fprintf(stderr, "[ERROR] [WORLD] [JSON] Failed to load scene!\n");
  cJSON_Delete(scene_json);
}

void world_destroy(World* world) {
  for (usize i = 0; i < world->hittables_count; i++) {
    world->hittables[i]->destroy(world->hittables[i]);
  }

  free(world->hittables);
  world->hittables = NULL;
}
