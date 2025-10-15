#include "world.h"

#include <stdlib.h>
#include <stdio.h>

#include "math/sphere.h"

World world_create() {
  World world = {0};

  world.capacity = WORLD_STARTING_CAPACITY;
  world.spheres = (Sphere*) malloc(sizeof(Sphere) * world.capacity);
  if (!world.spheres) {
    fprintf(stderr, "[ERROR] [WORLD] Failed to allocate memory for world!\n");
    return (World) {0};
  }

  return world;
}

void world_add(World* world, Sphere sphere) {
  if (world->sphere_count + 1 >= world->capacity) {
    Sphere* temp = (Sphere*) realloc(world->spheres, sizeof(Sphere) * (world->capacity * WORLD_SCALE_FACTOR));
    if (!temp) {
      fprintf(stderr, "[ERROR] [WORLD] Failed to reallocate memory for world!\n");
      return;
    }

    world->spheres = temp;
    world->capacity *= WORLD_SCALE_FACTOR;
  }

  world->spheres[world->sphere_count] = sphere;
  world->sphere_count++;
}

void world_destroy(World* world) {
  free(world->spheres);
  world->spheres = NULL;
}
