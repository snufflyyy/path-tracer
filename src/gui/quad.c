#include "gui/quad.h"

const GLfloat QUAD_VERTICES[] = { 
  0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
  -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
  -0.5f, 0.5f, 0.0f, 0.0f, 1.0f
};
const GLuint QUAD_INDICES[] = { 
  0, 1, 3,
  1, 2, 3
};

Quad quad_create() {
  Quad quad = {0};

  glGenVertexArrays(1, &quad.VAO);
  glGenBuffers(1, &quad.VBO);
  glGenBuffers(1, &quad.EBO);

  glBindVertexArray(quad.VAO);
  glBindBuffer(GL_ARRAY_BUFFER, quad.VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad.EBO);

  glBufferData(GL_ARRAY_BUFFER, sizeof(QUAD_VERTICES), QUAD_VERTICES, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*) 0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*) (sizeof(GLfloat) * 3));
  glEnableVertexAttribArray(1);

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(QUAD_INDICES), QUAD_INDICES, GL_STATIC_DRAW);

  return quad;
}

void quad_render(Quad quad) {
  glBindVertexArray(quad.VAO);
  glDrawElements(GL_TRIANGLES, (sizeof(QUAD_INDICES) / sizeof(GLuint)), GL_UNSIGNED_INT, 0);
}

void quad_destroy(Quad quad) {
  glDeleteBuffers(1, &quad.EBO);
  glDeleteBuffers(1, &quad.VBO);
  glDeleteVertexArrays(1, &quad.VAO);
}
