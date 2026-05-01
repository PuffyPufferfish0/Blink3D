CXX = g++

CXXFLAGS = -I./include -I./imgui -I. $(shell pkg-config --cflags sdl2)
LDFLAGS = -lSDL2 -ldl

SOURCES = main.cpp App.cpp glad.c Point.cpp Camera.cpp Mesh.cpp ViewCube.cpp Toolbar.cpp TransformGizmo.cpp Line.cpp Face.cpp SplitTool.cpp AddMenu.cpp imgui/*.cpp

all: clean build

build:
	$(CXX) $(SOURCES) $(CXXFLAGS) -o Blink3D $(LDFLAGS)
	@echo "Build complete! Run with: nix run --impure github:nix-community/nixGL -- ./Blink3D"

clean:
	rm -f Blink3D
	@echo "Cleaned old builds."

.PHONY: all build clean