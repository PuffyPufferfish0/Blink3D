# Blink3D
A 3D modeling software designed for triangulation and textures. 

for the make command, use common sense, it could change depending on added classes, or you may need to exclude some classes for debugging:

```
g++ main.cpp App.cpp glad.c Point.cpp Camera.cpp Mesh.cpp imgui/*.cpp -I./include -I./imgui -I/usr/include/SDL2 -I. -o App -lSDL2 -ldl

```

run after make

```
./App
```
### Nix compatible!
make sure you have nix installed 
```
sh <(curl --proto '=https' --tlsv1.2 -L https://nixos.org/nix/install) --daemon
```
```
nix develop
nix run --impure github:nix-community/nixGL -- ./Blink3D
```



## SHORTCUTS
|task|execution|
|-----|-------|
|**snap to front parametric**| **z**|
|**toggle point visability**|**p** |
