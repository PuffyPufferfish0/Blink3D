# Blink3D
### make sure to checkout [timeline](https://github.com/PuffyPufferfish0/Blink3D/blob/main/timeline.md)
A 3D modeling software designed for triangulation and textures. 



for the make command, use common sense, it will change depending on added classes, or you may need to exclude some classes for debugging:

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
you will then need to enable nix flakes
```
mkdir -p ~/.config/nix
echo "experimental-features = nix-command flakes" >> ~/.config/nix/nix.conf
```
```
nix develop
```

_run make command given from dev output_
**OR**
`make`




## SHORTCUTS
|task|execution|
|-----|-------|
|**snap to parametric**| **z**|
|**slide parametric**| **z + [arrow key]**|
|**toggle point visability**|**p** |
|**toggle grid visability**|**g**|
|**snap points**|**select point, s**|
|**swtich target mode**|**1,2,3**|
|**add menu**|**tab**|


### .configSettings file
- you must have the .configSetting IN the Blink3D directory for it to read
- make sure to export it from preferences menu


