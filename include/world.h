#pragma once

#include "hittables/hittable.h"
#include "types/base_types.h"
#include "types/color.h"

#define WORLD_STARTING_CAPACITY 5
#define WORLD_SCALE_FACTOR 2.0

#define DEFAULT_MAX_RAY_BOUNCES 10
#define DEFAULT_SKY_COLOR (Color) { 0.65f, 0.80f, 1.0f }

struct Camera;

typedef struct World {
  Hittable** hittables;
  u32 hittables_count;
  u32 capacity;

  bool indirect_light_sampling;
  bool direct_light_sampling;

  u32 max_ray_bounces;
  Color sky_color;
} World;

World world_create();
void world_add(World* world, Hittable* object);
void world_remove(World* world, usize index);

void world_scene_save(World* world, struct Camera* camera, const char* filename);
void world_scene_load(World* world, struct Camera* camera, const char* filename);

void world_destroy(World* world);
