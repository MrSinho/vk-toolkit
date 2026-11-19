{ inputs, pkgs, pipeline }:

let

  kdev4File = pkgs.writeText "vk-toolkit.kdev4" ''
    [Project]
    Name=vk-toolkit
    Manager=KDevCMakeManager
  '';

  env = {
    VK_TOOLKIT_GLFW_CMAKE_CONFIG_PATH = "${pkgs.glfw.dev}/lib/cmake/glfw3/glfw3Config.cmake";
    VK_TOOLKIT_SPIRV_REFLECT_ROOT_DIR = builtins.toString inputs.spirv-reflect;
    VK_TOOLKIT_BUILD_EXAMPLES = "ON";
  };

in
{
  
  default = pkgs.mkShell {
    nativeBuildInputs = pipeline.nativeBuildInputs;
    buildInputs = pipeline.buildInputs;

    inherit env;
  };

}