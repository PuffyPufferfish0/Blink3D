# Blink3D
A 3D modeling software designed for triangulation and textures. 

for the make command, use common sense, it could change depending on added classes, or you may need to exclude some classes for debugging:

```
g++ main.cpp App.cpp glad.c Point.cpp Camera.cpp Mesh.cpp ViewCube.cpp Toolbar.cpp TransformGizmo.cpp imgui/*.cpp -I./include -I./imgui -I. $(pkg-config --cflags sdl2) -o Blink3D -lSDL2 -ldl

```

run after make

```
./App
```

as of right now, it is HIGHLY recommended to use NIX because I'm not updating that build as often as the NIX build
### Nix compatible!
make sure you have nix installed 
```
sh <(curl --proto '=https' --tlsv1.2 -L https://nixos.org/nix/install) --daemon
```
```
nix develop
```
```
nix run --impure github:nix-community/nixGL -- ./Blink3D
```



## SHORTCUTS
|task|execution|
|-----|-------|
|**snap to parametric**| **z**|
|**slide parametric**| **z + [arrow key]**|
|**toggle point visability**|**p** |
