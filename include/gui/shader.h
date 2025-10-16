#pragma once

#include <glad/glad.h>

typedef GLuint Shader;

Shader shader_create(const char* vertex_shader_path, const char* fragment_shader_path);
void shader_bind(Shader shader);
void shader_unbind();
void shader_destroy(Shader shader);
