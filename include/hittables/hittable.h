#pragma once

#include "math/vector3.h"
#include "materials/material.h"
#include "types/rayhit.h"
#include "math/ray.h"

#define HITTABLE_IDENTIFER_MAX_LENGTH 16

typedef enum HittableType {
  SPHERE,
  PLANE
} HittableType;

typedef struct Hittable {
  HittableType type;
  char identifer[HITTABLE_IDENTIFER_MAX_LENGTH];

  Vector3* position;
  Material* material;

  RayHit (*hit)(struct Hittable* hittable, Ray ray);
  void (*destroy)(struct Hittable* hittable);
} Hittable;
