#include "gui/window.h"

#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <cimgui_impl.h>

#include "types/base_types.h"

Window window_create(u32 width, u32 height, const char* title) {
  if (!glfwInit()) {
    fprintf(stderr, "[ERROR] [GLFW] Failed to initialize GLFW!\n");
    return (Window) {0};
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_FALSE);
#endif

  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
  if (!window) {
    fprintf(stderr, "[ERROR] [GLFW] [WINDOW] Failed to create GLFW window!\n");
    return (Window) {0};
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
    fprintf(stderr, "[ERROR] [GLAD] Failed to load OpenGL!\n");
    return (Window) {0};
  }

  glViewport(0, 0, width, height);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  igCreateContext(NULL);

  ImGuiIO* imgui_io = igGetIO_Nil();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");

  return (Window) { window };
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
  igRender();
  ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
}

void window_destroy(Window *window) {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  igDestroyContext(NULL);

  glfwDestroyWindow(window->glfw_window);
  glfwTerminate();
}
