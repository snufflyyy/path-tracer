#include "gui/utils/file.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types/base_types.h"

const char* file_to_string(const char* path) {
  FILE* file = fopen(path, "r");
  if (!file) {
    fprintf(stderr, "[ERROR] [FILE] Failed to open file when converting file to string!\n");
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  s32 length = ftell(file);
  fseek(file, 0, SEEK_SET);

  char* string = (char*) malloc(length + 1); // + 1 bc null terminator
  if (!string) {
    fprintf(stderr, "[ERROR] [FILE] Failed to allocate memory for string when converting file to string!\n");
    return NULL;
  }

  u32 bytes_read = fread(string, 1, length, file);
  string[bytes_read] = '\0';

  fclose(file);

  return string;
}

void file_destroy_string(const char* string) {
  free((void*) string);
}
