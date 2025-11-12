#include "utils/file.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nfd.h"
#include "types/base_types.h"

const char* file_to_string(const char* path) {
  FILE* file = fopen(path, "r");
  if (!file) {
    fprintf(stderr, "[ERROR] [FILE] Failed to open file when converting file to string!\n");
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  u32 length = ftell(file);
  fseek(file, 0, SEEK_SET);

  char* string = (char*) malloc(length + 1); // +1 for null terminator
  if (!string) {
    fprintf(stderr, "[ERROR] [FILE] Failed to allocate memory for string when converting file to string!\n");
    fclose(file);
    return NULL;
  }

  u32 bytes_read = fread(string, 1, length, file);
  string[bytes_read] = '\0';

  fclose(file);

  return string;
}

void file_write_string(const char* path, const char* string) {
  FILE* file = fopen(path, "w");
  if (!file) {
    fprintf(stderr, "[ERROR] [FILE] Failed to open file when writing string to file!\n");
    return;
  }

  if (fputs(string, file) < 0) {
    fprintf(stderr, "[ERROR] [FILE] Failed to write string to file!\n");
  }

  fclose(file);
}

const char* file_dialog_get_open(nfdfilteritem_t* filter_items, u32 filter_items_count) {
  if (NFD_Init() == NFD_ERROR) {
    fprintf(stderr, "[ERROR] [FILE] [DIALOG] [OPEN] Failed to initalize native file dialogs!\n");
    return NULL;
  }

  nfdu8char_t *open_path;
  nfdresult_t result = NFD_OpenDialog(&open_path, filter_items, filter_items_count, NULL);
  if (result == NFD_ERROR) {
    fprintf(stderr, "[ERROR] [FILE] [DIALOG] [OPEN] Failed to get path!\n");
    NFD_Quit();
    return NULL;
  }
  NFD_Quit();

  if (result == NFD_CANCEL) {
    return "";
  }

  return (const char*) open_path;
}

const char* file_dialog_get_save(nfdfilteritem_t* filter_items, u32 filter_items_count) {
  if (NFD_Init() == NFD_ERROR) {
    fprintf(stderr, "[ERROR] [FILE] [DIALOG] [SAVE] Failed to initalize native file dialogs!\n");
    return NULL;
  }

  nfdu8char_t *save_path;
  nfdresult_t result = NFD_SaveDialogU8(&save_path, filter_items, filter_items_count, NULL, NULL);
  if (result == NFD_ERROR) {
    fprintf(stderr, "[ERROR] [FILE] [DIALOG] [SAVE] Failed to get path!\n");
    NFD_Quit();
    return NULL;
  }

  NFD_Quit();

  if (result == NFD_CANCEL) {
    return "";
  }

  return (const char*) save_path;
}

inline void file_dialog_string_destroy(const char* string) {
  NFD_FreePathU8((nfdu8char_t*) string);
}
