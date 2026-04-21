{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  packages = [
    pkgs.clang
    pkgs.cmake
    pkgs.ninja
    pkgs.pkg-config
    pkgs.libGL
    pkgs.libX11.dev
    pkgs.wayland
    pkgs.wayland-protocols
    pkgs.wayland-scanner
    pkgs.libxkbcommon
    pkgs.libxrandr
    pkgs.libxinerama
    pkgs.libxcursor
    pkgs.libxi
    pkgs.gtk3
  ];
}
