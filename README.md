# Path Tracer
(Version 0.5 INCOMPLETE)

My amazing CPU (and GPU one day... maybe) path tracer that is written in c with a gui built with glfw, gald (OpenGL), and cimgui.

## Screenshots

todo

## Final Renders

todo

## Features

 - Relatively fast when using multithreading
 - Easy to use UI
 - Scene saving and loading
 - Material system
 - Multithreading
 - Image (JPG) exporting
 - todo

## How to Build and Run

This project uses CMake as it's build system, has all it's dependences included already (or as a git submodule), and includes build scripts for debug and release builds (that work only on Linux and MacOS).

### Build and Run

First, clone the repository and navigate into it:

```bash
git clone --recurse-submodule https://github.com/snufflyyy/path-tracer.git
cd path-tracer
```

Next, if you are just going to use the path tracer, you should run the run-release.sh:

```bash
chmod +x build-release.sh run-release.sh
./run-release.sh
```

or if you are going to work with the code, you should run the run-debug.sh:

```bash
chmod +x build-debug.sh run-debug.sh
./run-debug.sh
```

## Usage

The GUI should be pretty self-explanatory if you have used any 3D software before, if you don't know what to do just start messing with things and seeing what renders you can create, it can be a little fun :)

## Acknowledgements

 - [stb_image_write.h](https://github.com/nothings/stb) - used for final render image exporting
 - [GLFW](https://www.glfw.org) - used for creating windows and my OpenGL context
 - [glad](https://glad.dav1d.de) - used for exposing the OpenGL bindings
 - [cimgui](https://github.com/cimgui/cimgui) - used for creating the ui by letting me use the amazing [dear imgui](https://github.com/ocornut/imgui) ui library

## License

[MIT](https://choosealicense.com/licenses/mit/)
