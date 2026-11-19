{

  inputs = {

    nixpkgs = {
      url = "github:nixos/nixpkgs/nixos-unstable";
    };

    flake-utils = {
      url = "github:numtide/flake-utils";
    };

    spirv-reflect = {
      url = "github:KhronosGroup/SPIRV-Reflect";
      flake = false;
    };

  };

  outputs = inputs @ { nixpkgs, flake-utils, ... }: flake-utils.lib.eachDefaultSystem (system:
    let
    
      pkgs = import nixpkgs { inherit system; };

      pipeline = import ./nix/pipeline.nix { inherit inputs pkgs; };

    in rec {

      packages = {
        vk-toolkit = pipeline.vk-toolkit;
        default = packages.vk-toolkit;
      };

      devShells = import ./nix/devShells.nix { inherit inputs pkgs pipeline; };

    }

  );

}
