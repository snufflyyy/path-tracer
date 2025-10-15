#pragma once

#include "math/sphere.h"
#include "types/base_types.h"

#define WORLD_STARTING_CAPACITY 10
#define WORLD_SCALE_FACTOR 2.0

typedef struct World {
  Sphere* spheres;
  u32 sphere_count;
  u32 capacity;
} World;

World world_create();
void world_add(World* world, Sphere sphere);
void world_destroy(World* world);
