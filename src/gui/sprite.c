#include "gui/sprite.h"
#include "gui/quad.h"
#include "gui/shader.h"
#include "gui/texture.h"

Sprite sprite_create(Shader shader, Texture texture) {
  Sprite sprite = {0};

  sprite.quad = quad_create();
  sprite.shader = shader;
  sprite.texture = texture;

  return sprite;
}

void sprite_render(Sprite* sprite) {
  shader_bind(&sprite->shader);
  texture_bind(sprite->texture);
  quad_render(sprite->quad);
}

void sprite_destroy(Sprite* sprite) {
  shader_destroy(&sprite->shader);
  texture_destroy(sprite->texture);
  quad_destroy(sprite->quad);
}
