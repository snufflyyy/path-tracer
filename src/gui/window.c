#include "gui/window.h"

#include <stdio.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <cimgui_impl.h>

#include "types/base_types.h"

static void window_resize_callback(GLFWwindow* window, s32 width, s32 height);

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

  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

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

  glfwSetFramebufferSizeCallback(window->glfw_window, window_resize_callback);

  if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
    fprintf(stderr, "[ERROR] [GLAD] Failed to load OpenGL!\n");
    glfwDestroyWindow(window->glfw_window);
    glfwTerminate();
    return NULL;
  }

  glViewport(0, 0, width, height);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  window->imgui_context = igCreateContext(NULL);
  if (!window->imgui_context) {
    fprintf(stderr, "[ERROR] [WINDOW] Failed to create ImGui context!\n");
    glfwDestroyWindow(window->glfw_window);
    glfwTerminate();
    return NULL;
  }

  ImGuiIO* imgui_io = igGetIO_ContextPtr(window->imgui_context);
  if (!imgui_io) {
    fprintf(stderr, "[ERROR] [WINDOW] Failed to get ImGui I/O!\n");
    glfwDestroyWindow(window->glfw_window);
    glfwTerminate();
    return NULL;
  }

  imgui_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  #ifdef IMGUI_HAS_DOCK
    imgui_io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  #endif

  if (!ImGui_ImplGlfw_InitForOpenGL(window->glfw_window, true)) {
    fprintf(stderr, "[ERROR] [WINDOW] [IMGUI] Failed to initialize ImGui for GLFW for OpenGL!\n");
    glfwDestroyWindow(window->glfw_window);
    glfwTerminate();
    return NULL;
  }

  if (!ImGui_ImplOpenGL3_Init("#version 330")) {
    fprintf(stderr, "[ERROR] [WINDOW] [IMGUI] Failed to initialize ImGui for OpenGL!\n");
    glfwDestroyWindow(window->glfw_window);
    glfwTerminate();
    return NULL;
  }

  return window;
}

static void window_resize_callback(GLFWwindow* window, s32 width, s32 height) {
  glViewport(0, 0, width, height);
}

inline bool window_is_running(Window* window) {
  return !glfwWindowShouldClose(window->glfw_window);
}

inline void window_set_is_running(Window* window, bool value) {
  glfwSetWindowShouldClose(window->glfw_window, !value);
}

inline void window_update(Window* window) {
  glfwSwapBuffers(window->glfw_window);
  glfwPollEvents();
}

inline void window_clear(Window* window) {
  glClear(GL_COLOR_BUFFER_BIT);
}

inline void window_imgui_begin_frame() {
  ImGui_ImplGlfw_NewFrame();
  ImGui_ImplOpenGL3_NewFrame();
  igNewFrame();
}

inline void window_imgui_end_frame() {
  igEndFrame();
}

inline void window_imgui_render() {
  igRender();
  ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData()); 
}

void window_destroy(Window *window) {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  igDestroyContext(window->imgui_context);

  glfwDestroyWindow(window->glfw_window);

  free(window);

  glfwTerminate();
}
