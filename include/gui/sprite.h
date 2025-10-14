#pragma once

#include "gui/quad.h"
#include "gui/texture.h"
#include "gui/shader.h"

typedef struct Sprite {
  Quad quad;
  Shader shader;
  Texture texture;
} Sprite;

Sprite sprite_create(Shader shader, Texture texture);
void sprite_render(Sprite* sprite);
void sprite_destroy(Sprite* sprite);
