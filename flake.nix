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
            echo " Blink3D Development Environment Loaded!"
            echo " To connect to your Linux Mint GPU, run:"
            echo "nix run --impure github:nix-community/nixGL -- ./Blink3D"
          '';
        };
      });
    };
}
