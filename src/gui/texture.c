#include "gui/texture.h"

#include <glad/glad.h>

#include "types/color.h"
#include "types/base_types.h"

GLTexture texture_create() {
  GLTexture texture;
  glGenTextures(1, &texture);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return texture;
}

inline void texture_bind(GLTexture texture) {
  glBindTexture(GL_TEXTURE_2D, texture);
}

inline void texture_unbind() {
  glBindTexture(GL_TEXTURE_2D, 0);
}

inline void texture_set_colorRGB_buffer(GLTexture texture, ColorRGB* buffer, u32 width, u32 height) {
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
}

inline void texture_destroy(GLTexture texture) {
  glDeleteTextures(1, &texture);
}
