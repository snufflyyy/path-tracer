#include "gui/shader.h"

#include <stdio.h>

#include <glad/glad.h>

#include "gui/utils/file.h"
#include "types/base_types.h"

#define SHADER_RESULT_BUFFER_SIZE 512

static GLuint create_individual_shader(const char* shader_source, GLenum shader_type);

Shader shader_create(const char* vertex_shader_path, const char* fragment_shader_path) {
  Shader shader = {0};

  const char* vertex_shader_source = file_to_string(vertex_shader_path);
  if (!vertex_shader_source) {
    fprintf(stderr, "[ERROR] [SHADER] Failed to open vertex shader source file!\n");
    return shader;
  }

  const char* fragment_shader_source = file_to_string(fragment_shader_path);
  if (!fragment_shader_source) {
    fprintf(stderr, "[ERROR] [SHADER] Failed to open fragment shader source file!\n");
    return shader;
  }

  GLuint vertex_shader = create_individual_shader(vertex_shader_source, GL_VERTEX_SHADER);
  GLuint fragment_shader = create_individual_shader(fragment_shader_source, GL_FRAGMENT_SHADER);

  file_destroy_string(vertex_shader_source);
  file_destroy_string(fragment_shader_source);

  shader.id = glCreateProgram();
  glAttachShader(shader.id, vertex_shader);
  glAttachShader(shader.id, fragment_shader);
  glLinkProgram(shader.id);

  s32 shader_result;
  char shader_result_buffer[SHADER_RESULT_BUFFER_SIZE];
  glGetProgramiv(shader.id, GL_LINK_STATUS, &shader_result);
  if (!shader_result) {
    glGetProgramInfoLog(shader.id, SHADER_RESULT_BUFFER_SIZE, NULL, shader_result_buffer);
    fprintf(stderr, "[ERROR] [SHADER] Failed to link shader program:\n");
    fprintf(stderr, "%s", shader_result_buffer);
  }

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  return shader;
}

void shader_bind(Shader* shader) {
  glUseProgram(shader->id);
}

void shader_unbind() {
  glUseProgram(0);
}

void shader_destroy(Shader* shader) {
  glDeleteProgram(shader->id);
}

static GLuint create_individual_shader(const char* shader_source, GLenum shader_type) {
  s32 shader_result;
  char shader_result_buffer[SHADER_RESULT_BUFFER_SIZE];

  GLuint shader = glCreateShader(shader_type);

  glShaderSource(shader, 1, &shader_source, NULL);
  glCompileShader(shader);

  glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_result);
  if (!shader_result) {
    glGetShaderInfoLog(shader, SHADER_RESULT_BUFFER_SIZE, NULL, shader_result_buffer);

    char* type;
    switch (shader_type) {
      case GL_VERTEX_SHADER:
        type = "vertex";
        break;
      case GL_FRAGMENT_SHADER:
        type = "fragment";
        break;
      default:
        type = "unknown";
        break;
     }

    fprintf(stderr, "[ERROR] [SHADER] Failed to compile %s shader:\n", type);
    fprintf(stderr, "%s", shader_result_buffer);
  }

    return shader;
}
