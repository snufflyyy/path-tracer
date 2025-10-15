#include "world.h"

#include <stdlib.h>
#include <stdio.h>

#include "math/sphere.h"

World world_create() {
  World world = {0};

  world.capacity = WORLD_STARTING_CAPACITY;
  world.hittables = (Sphere*) malloc(sizeof(Sphere) * world.capacity);
  if (!world.hittables) {
    fprintf(stderr, "[ERROR] [WORLD] Failed to allocate memory for world!\n");
    return (World) {0};
  }

  world.max_ray_bounces = DEFAULT_MAX_RAY_BOUNCES;
  world.sky_color = DEFAULT_SKY_COLOR;

  return world;
}

void world_add(World* world, Hittable sphere) {
  if (world->hittables_count + 1 >= world->capacity) {
    Hittable* temp = (Hittable*) realloc(world->hittables, sizeof(Hittable) * (world->capacity * WORLD_SCALE_FACTOR));
    if (!temp) {
      fprintf(stderr, "[ERROR] [WORLD] Failed to reallocate memory while increasing world capacity!\n");
      return;
    }

    world->hittables = temp;
    world->capacity *= WORLD_SCALE_FACTOR;
  }

  world->hittables[world->hittables_count] = sphere;
  world->hittables_count++;
}

void world_remove(World* world, usize index) {
  if (world->hittables_count <= world->capacity / WORLD_SCALE_FACTOR) {
    Hittable* temp = (Hittable*) realloc(world->hittables, sizeof(Hittable) * (world->capacity / WORLD_SCALE_FACTOR));
    if (!temp) {
      fprintf(stderr, "[ERROR] [WORLD] [Failed to reallocate memory while shrinking world capacity!\n");
      return;
    }

    world->hittables = temp;
    world->capacity /= WORLD_SCALE_FACTOR;
  }

  if (index > world->hittables_count) { return; }
  if (index == world->hittables_count - 1) {
    world->hittables_count--;
    return;
  }

  for (usize i = index; i < world->hittables_count - 1; i++) {
    world->hittables[i] = world->hittables[i + 1];
  }
  world->hittables--;
}

void world_destroy(World* world) {
  free(world->hittables);
  world->hittables = NULL;
}
