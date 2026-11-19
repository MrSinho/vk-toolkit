{
  pkgs,
  spirv-reflect ? builtins.fetchGit {
    url = "https://github.com/KhronosGroup/SPIRV-Reflect";
  },
  ...
}:

let
  inputs = { inherit spirv-reflect; };
  pipeline = import ./pipeline.nix { inherit inputs pkgs; };
in

pipeline."vk-toolkit"
