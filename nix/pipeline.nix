{ inputs, pkgs, ... }:

let
  nativeBuildInputs = [
    pkgs.cmake
    pkgs.ninja
    pkgs.clang
    pkgs.pkg-config
  ];

  buildInputs = [
    pkgs.vulkan-tools
    pkgs.vulkan-loader
    pkgs.vulkan-helper
    pkgs.vulkan-headers
    
    pkgs.shader-slang
    pkgs.spirv-tools

    pkgs.glfw
    
    pkgs.kdePackages.wayland
    pkgs.kdePackages.wayland-protocols
    pkgs.wayland-scanner
    
    pkgs.libxkbcommon.dev
    pkgs.libX11.dev
    pkgs.libXi.dev
    pkgs.libXrandr.dev
    pkgs.libXinerama.dev
    pkgs.libXcursor.dev
    pkgs.libXext.dev
    
    pkgs.doxygen
  ];

  cmakeDefaultFlags = [
    "-DVK_TOOLKIT_BUILD_EXAMPLES=ON"
    #"-DVK_TOOLKIT_BUILD_DOCS=ON"
    "-DVK_TOOLKIT_SPIRV_REFLECT_ROOT_DIR=${builtins.toString inputs.spirv-reflect}"
    "-DVK_TOOLKIT_INSTALL_DATADIR=${placeholder "out"}/share/"
  ];

  cmakeFlags = cmakeDefaultFlags ++ [
    "-Wno-dev"
    "-U'*'"
  ];

  # Shoguldn't be needed anymore if the dev outputs have been created correctly
  LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath (buildInputs); # Allows linking to shared libraries (e.g. through dlopen)

  vk-toolkit = pkgs.stdenv.mkDerivation {
    pname = "vk-toolkit";
    version = "1.1.1";

    src = ./../.;

    outputs = [ "out" "dev" ]; # Pointed by $out and $dev

    inherit nativeBuildInputs buildInputs cmakeFlags;
  };

in
{
  
  inherit vk-toolkit cmakeDefaultFlags nativeBuildInputs buildInputs;

}
