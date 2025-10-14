#pragma once

#include <stdbool.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "types/base_types.h"

typedef struct Window {
  GLFWwindow* glfw_window;
} Window;

Window window_create(u32 width, u32 height, const char* title);
bool window_is_running(Window* window);
void window_update(Window* window);
void window_clear(Window* window);
void window_imgui_begin_frame();
void window_imgui_end_frame();
void window_destroy(Window* window);
