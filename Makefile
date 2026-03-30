# Makefile for Blink3D Engine

CXX = g++

# $(shell ...) guarantees this is evaluated correctly regardless of bash/zsh/fish
CXXFLAGS = -I./include -I./imgui -I. $(shell pkg-config --cflags sdl2)
LDFLAGS = -lSDL2 -ldl

SOURCES = main.cpp App.cpp glad.c Point.cpp Camera.cpp Mesh.cpp ViewCube.cpp Toolbar.cpp TransformGizmo.cpp imgui/*.cpp

# The default rule
Blink3D:
	$(CXX) $(SOURCES) $(CXXFLAGS) -o Blink3D $(LDFLAGS)

clean:
	rm -f Blink3D
