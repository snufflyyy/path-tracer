cmake_minimum_required(VERSION 3.16)
project(PathTracer)
if (APPLE)
  enable_language(OBJC OBJCXX)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  find_package(X11 REQUIRED)
endif()

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

add_library(cJSON external/cJSON/src/cJSON.c)
target_include_directories(cJSON PUBLIC external/cJSON/include)

set(NFD_BUILD_TESTS OFF)
add_subdirectory(external/nativefiledialog-extended)
if(APPLE)
  target_link_libraries(nfd PRIVATE
    "-framework AppKit"
    "-framework UniformTypeIdentifiers"
  )
endif()

add_executable(
  ${PROJECT_NAME}
  src/main.c
  src/types/color.c
  src/image.c
  src/random.c
  src/viewport.c
  src/tonemapping.c
  src/camera.c
  src/world.c

  src/utils/file.c

  src/math/vector3.c
  src/math/ray.c

  src/textures/texture.c
  src/textures/solid_color.c
  src/textures/image.c

  src/materials/diffuse.c
  src/materials/metal.c
  src/materials/glass.c
  src/materials/emissive.c

  src/hittables/sphere.c
  src/hittables/plane.c

  src/gui/gui.c
  src/gui/window.c
  src/gui/texture.c

  external/cimgui/imgui/backends/imgui_impl_glfw.cpp
  external/cimgui/imgui/backends/imgui_impl_opengl3.cpp
)

include_directories(
  include

  external/stb_image_write/include
  external/stb_image/include
)

target_compile_definitions(${PROJECT_NAME} PUBLIC -DCIMGUI_USE_OPENGL3 -DCIMGUI_USE_GLFW)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw ${OpenGL_LIBRARIES} glad cimgui cJSON nfd)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_link_libraries(${PROJECT_NAME} PRIVATE ${X11_LIBRARIES})
endif()
