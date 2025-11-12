#pragma once

#include <nfd.h>

#include "types/base_types.h"

const char* file_to_string(const char* filename);
void file_write_string(const char* path, const char* string);

const char* file_dialog_get_open(nfdfilteritem_t* filter_items, u32 filter_items_count);
const char* file_dialog_get_save(nfdfilteritem_t* filter_items, u32 filter_items_count);
void file_dialog_string_destroy(const char* string);
