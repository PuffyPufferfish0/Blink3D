{
  description = "Blink3D Engine Development Environment";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      supportedSystems = [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin" ];
      forAllSystems = nixpkgs.lib.genAttrs supportedSystems;
      pkgs = forAllSystems (system: import nixpkgs { inherit system; });
    in
    {
      devShells = forAllSystems (system: {
        default = pkgs.${system}.mkShell {
          
          buildInputs = with pkgs.${system}; [
            gcc
            SDL2
            glm
            libGL
            pkg-config
            libx11
          ];

          shellHook = ''
            echo "Blink3D Nix Environment Loaded!"
            echo "---------------------------------------------------------"
            echo "To compile with the new class system, use this command:"
            echo "g++ main.cpp App.cpp glad.c Point.cpp Camera.cpp Mesh.cpp ViewCube.cpp Toolbar.cpp TransformGizmo.cpp imgui/*.cpp -I./include -I./imgui -I. \$(pkg-config --cflags sdl2) -o Blink3D -lSDL2 -ldl"
            echo "---------------------------------------------------------"
            echo "To connect to your Linux GPU, run:"
            echo "nix run --impure github:nix-community/nixGL -- ./Blink3D"
          '';
        };
      });
    };
}
