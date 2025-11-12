#pragma once

#include <glad/glad.h>

#include "types/color.h"
#include "types/base_types.h"

typedef GLuint GLTexture;

GLTexture texture_create();

void texture_bind(GLTexture texture);
void texture_unbind();

void texture_set_colorRGB_buffer(GLTexture texture, ColorRGB* buffer, u32 width, u32 height);

void texture_destroy(GLTexture texture);
