#pragma once

#include "math/sphere.h"
#include "types/base_types.h"
#include "types/color.h"

#define WORLD_STARTING_CAPACITY 10
#define WORLD_SCALE_FACTOR 2.0

#define DEFAULT_MAX_RAY_BOUNCES 10
#define DEFAULT_SKY_COLOR (Color) { 0.35f, 0.45f, 0.6f }

typedef Sphere Hittable; // temp

typedef struct World {
  Hittable* hittables;
  u32 hittables_count;
  u32 capacity;

  u32 max_ray_bounces;
  Color sky_color;
} World;

World world_create();
void world_add(World* world, Sphere sphere);
void world_remove(World* world, usize index);
void world_destroy(World* world);
