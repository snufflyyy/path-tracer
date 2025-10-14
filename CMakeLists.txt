cmake_minimum_required(VERSION 4.0)
project(PathTracer)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)

add_library(glad external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
add_subdirectory(external/glfw)

set(CIMGUI_USE_GLFW)
set(CIMGUI_USE_OPENGL3)
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
  src/types/color.c
  src/image.c
)

include_directories(
  include
  external/stb_image_write/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE glad glfw OpenGL::GL cimgui)

