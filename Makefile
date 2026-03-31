# Makefile for Blink3D Engine

CXX = g++

# $(shell ...) guarantees this is evaluated correctly regardless of bash/zsh/fish
CXXFLAGS = -I./include -I./imgui -I. $(shell pkg-config --cflags sdl2)
LDFLAGS = -lSDL2 -ldl

# Added AddMenu.cpp
SOURCES = main.cpp App.cpp glad.c Point.cpp Camera.cpp Mesh.cpp ViewCube.cpp Toolbar.cpp TransformGizmo.cpp Line.cpp Face.cpp SplitTool.cpp AddMenu.cpp imgui/*.cpp

# 'all' is the default target when you just type 'make'
all: clean build

# The actual compilation step
build:
	$(CXX) $(SOURCES) $(CXXFLAGS) -o Blink3D $(LDFLAGS)
	@echo "Build complete! Run with: nix run --impure github:nix-community/nixGL -- ./Blink3D"

# Deletes the old binary
clean:
	rm -f Blink3D
	@echo "Cleaned old builds."

# .PHONY tells Make that these aren't actual files, so it should always run their commands
.PHONY: all build clean