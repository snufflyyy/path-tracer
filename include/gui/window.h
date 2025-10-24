#pragma once

#include <stdbool.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>

#include "types/base_types.h"

typedef struct Window {
  GLFWwindow* glfw_window;
  ImGuiContext* imgui_context;
} Window;

Window* window_create(u32 width, u32 height, const char* title);

bool window_is_running(Window* window);
void window_set_is_running(Window* window, bool value);

void window_update(Window* window);

void window_clear(Window* window);

void window_imgui_begin_frame();
void window_imgui_end_frame();
void window_imgui_render();

void window_destroy(Window* window);
