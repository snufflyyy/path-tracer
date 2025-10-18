#include "gui/window.h"

#include <stdio.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <cimgui_impl.h>

#include "types/base_types.h"

static void window_resize_callback(GLFWwindow* window, int width, int height) {
  Window* window_struct = (Window*) glfwGetWindowUserPointer(window);
  glViewport(0, 0, width, height);
}

Window* window_create(u32 width, u32 height, const char* title) {
  if (!glfwInit()) {
    fprintf(stderr, "[ERROR] [GLFW] Failed to initialize GLFW!\n");
    return NULL;
  }

  Window* window = (Window*) malloc(sizeof(Window));
  if (!window) {
    fprintf(stderr, "[ERROR] [WINDOW] Failed to allocate memory for window!\n");
    glfwTerminate();
    return NULL;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_FALSE);
  #endif

  window->glfw_window = glfwCreateWindow(width, height, title, NULL, NULL);
  if (!window->glfw_window) {
    fprintf(stderr, "[ERROR] [GLFW] [WINDOW] Failed to create GLFW window!\n");
    glfwTerminate();
    return NULL;
  }
  glfwMakeContextCurrent(window->glfw_window);

  glfwSetWindowUserPointer(window->glfw_window, (void*) window);
  glfwSetFramebufferSizeCallback(window->glfw_window, window_resize_callback);

  if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
    fprintf(stderr, "[ERROR] [GLAD] Failed to load OpenGL!\n");
    glfwDestroyWindow(window->glfw_window);
    glfwTerminate();
    return NULL;
  }

  glViewport(0, 0, width, height);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  igCreateContext(NULL);

  ImGuiIO* imgui_io = igGetIO_Nil();
  #ifdef IMGUI_HAS_DOCK
    imgui_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable;
  #endif

  ImGui_ImplGlfw_InitForOpenGL(window->glfw_window, true);
  ImGui_ImplOpenGL3_Init("#version 330");

  return window;
}

void window_set_is_running(Window* window, bool value) {
  glfwSetWindowShouldClose(window->glfw_window, !value);
}

bool window_is_running(Window* window) {
  return !glfwWindowShouldClose(window->glfw_window);
}

void window_update(Window* window) {
  glfwSwapBuffers(window->glfw_window);
  glfwPollEvents();
}

void window_clear(Window* window) {
  glClear(GL_COLOR_BUFFER_BIT);
}

void window_imgui_begin_frame() {
  ImGui_ImplGlfw_NewFrame();
  ImGui_ImplOpenGL3_NewFrame();
  igNewFrame();
}

void window_imgui_end_frame() {
  igEndFrame();
}

void window_imgui_render() {
  igRender();
  ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
}

void window_destroy(Window *window) {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  igDestroyContext(NULL);

  glfwDestroyWindow(window->glfw_window);

  free(window);

  glfwTerminate();
}
