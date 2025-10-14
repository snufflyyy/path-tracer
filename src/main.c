#include "gui/shader.h"
#include "types/color.h"

#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS 
#include <cimgui.h>
#include <cimgui_impl.h>

#include "camera.h"
#include "gui/window.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

const GLfloat vertices[] = {
  1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
  1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
 -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
 -1.0f, 1.0f, 0.0f,   0.0f, 1.0f
};
const GLuint indices[] = {
    0, 1, 3,
    1, 2, 3
};

int main() {
  Camera camera = camera_create(WINDOW_WIDTH, WINDOW_HEIGHT);

  ColorRGB framebufferRGB[WINDOW_WIDTH * WINDOW_HEIGHT];
  for (usize i = 0; i < camera.width * camera.height; i++) {
    camera.framebuffer[i].red = 1.0f;

    framebufferRGB[i] = color_convert_to_rgb(&camera.framebuffer[i]);
  }

  Window window = window_create(WINDOW_WIDTH, WINDOW_HEIGHT, "Path Tracer");
  Shader shader = shader_create("../shaders/shader.vert", "../shaders/shader.frag");

  GLuint VAO, VBO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*) 0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*) (sizeof(GLfloat) * 3));
  glEnableVertexAttribArray(1);

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glBindVertexArray(0); // this also unbinds array buffer and element buffer

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  GLuint texture;
  glGenTextures(1, &texture);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, camera.width, camera.height, 0, GL_RGB, GL_UNSIGNED_BYTE, framebufferRGB);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  while (window_is_running(&window)) {
    window_update(&window);

    window_clear(&window);

    shader_bind(&shader);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    window_imgui_begin_frame();
    igShowDemoWindow(NULL);
    window_imgui_end_frame();
  }
  shader_destroy(&shader);
  window_destroy(&window);
}
