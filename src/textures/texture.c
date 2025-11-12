#include "textures/texture.h"

#include "stdio.h"

#include "textures/image.h"
#include "textures/solid_color.h"

cJSON* texture_json_create(Texture* texture) {
  cJSON* texture_json = NULL;
  switch (texture->type) {
    case TEXTURE_TYPE_SOLID_COLOR: texture_json = texture_solid_color_json_create((TextureSolidColor*) texture); break;
    case TEXTURE_TYPE_IMAGE: texture_json = texture_image_json_create((TextureImage*) texture); break;
  }

  if (!texture_json) {
    fprintf(stderr, "[ERROR] [TEXTURE] [JSON] Failed to create json!\n");
    return NULL;
  }

  return texture_json;
}

Texture* texture_json_parse(cJSON* texture_json) {
  Texture* texture = NULL;
  switch ((TextureType) cJSON_GetNumberValue(cJSON_GetObjectItemCaseSensitive(texture_json, "type"))) {
    case TEXTURE_TYPE_SOLID_COLOR: texture = (Texture*) texture_solid_color_json_parse(texture_json); break;
    case TEXTURE_TYPE_IMAGE: texture = (Texture*) texture_image_json_parse(texture_json); break;
  }

  if (!texture) {
    fprintf(stderr, "[ERROR] [TEXTURE] [JSON] Failed to parse json!\n");
    return NULL;
  }

  return texture;
}
