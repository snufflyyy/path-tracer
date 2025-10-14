#pragma once

#include <glad/glad.h>

typedef struct Quad {
  GLuint VAO, VBO, EBO;
} Quad;

Quad quad_create();
void quad_render(Quad quad);
void quad_destroy(Quad quad);
