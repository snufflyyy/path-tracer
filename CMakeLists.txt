cmake_minimum_required(VERSION 4.0)
project(PathTracer)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)
#find_package(X11 REQUIRED)

add_library(glad external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
add_subdirectory(external/glfw)

add_subdirectory(external/cimgui)
target_include_directories(cimgui PUBLIC
  external/cimgui
  external/cimgui/generator/output
  external/cimgui/imgui
  external/cimgui/imgui/backends
)

add_executable(
  ${PROJECT_NAME}
  src/main.c
  src/types/base_types.c
  src/types/color.c
  src/image.c
  src/viewport.c
  src/camera.c
  src/world.c

  src/math/vector3.c
  src/math/ray.c
  src/math/sphere.c

  src/gui/gui.c
  src/gui/utils/file.c
  src/gui/window.c
  src/gui/shader.c
  src/gui/texture.c
  src/gui/quad.c
  src/gui/sprite.c

  external/cimgui/imgui/backends/imgui_impl_glfw.cpp
  external/cimgui/imgui/backends/imgui_impl_opengl3.cpp
)

include_directories(
  include
  external/stb_image_write/include
)

target_compile_definitions(${PROJECT_NAME} PUBLIC -DCIMGUI_USE_OPENGL3 -DCIMGUI_USE_GLFW)
#target_link_libraries(${PROJECT_NAME} PRIVATE ${X11_LIBRARIES} glfw ${OpenGL_LIBRARIES} glad cimgui)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw ${OpenGL_LIBRARIES} glad cimgui)
