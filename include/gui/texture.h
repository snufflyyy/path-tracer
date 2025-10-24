#pragma once

#include <glad/glad.h>

#include "types/color.h"
#include "types/base_types.h"

typedef GLuint Texture;

Texture texture_create();

void texture_bind(Texture texture);
void texture_unbind();

void texture_set_colorRGB_buffer(Texture texture, ColorRGB* buffer, u32 width, u32 height);

void texture_destroy(Texture texture);
