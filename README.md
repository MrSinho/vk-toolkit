# vk-toolkit

[![GitHub repository](https://img.shields.io/badge/GitHub-vk--toolkit-181717?style=for-the-badge&logo=github)](https://github.com/mrsinho/vk-toolkit)
![License](https://img.shields.io/github/license/mrsinho/shvulkan?style=for-the-badge)

vk-toolkit is a small set of libraries and examples to assist writing high-performance C/C++ Vulkan applications, without losing complexity.

> The project has moved from the old `shvulkan` wrapper layout to separate `vk-shell` (focused on the Vulkan API) and `vk-bridge` (for shader reflection) libraries. It uses Slang for the example shaders and SPIRV-Reflect for shader metadata. `vk-bridge` supplies helpers for reflection merging and Vulkan layout creation; graphics and compute pipeline creation stays in the application.

## Features

- Helpers for common Vulkan setup and API operations using `shvulkan`.
- Shader reflection using SPIRV-Reflect and `vk-bridge` for a simpler interface when creating pipelines.
- Application control over Vulkan, Slang, and SPIRV-Reflect APIs.
- CMake and Nix build support.
- Vulkan dynamic rendering, dynamic state, and Synchronization 2 demonstrated in the examples.
- Examples for clear-color rendering, a reflected Slang scene, and compute dispatch.

### Shader reflection

- [x] Entry points (partially).
- [x] Descriptor sets.
- [x] Push-constant ranges.
- [x] Vertex inputs (partially); composite struct inputs still need support.
- [x] Specialization constants.
- [ ] Atomics
- [ ] Textures.
- [ ] Dispatch local size.

## Roadmap

- Android build using the same codebase (Android NDK).
- Web build using the same codebase.
- Improve vertex-input reflection: flatten composite types, support struct inputs, and use reflected location and binding data to determine input layout.
- Atomic reflectoin, demonstrated with a quad whose fragments follow a sequence without interpolation, or with a simpler compute example.
- Add a tessellation state example.
- Add a geometry shader example.
- Add a PBR shader example.
- Test stencil attachments.
- Add a model-inference example.
- Consolidate the reusable example utilities under a clearly named common layer and document why each helper exists.

The Nix and CMake build path, Slang shader compilation in the examples, the CMake package export, and application-controlled error reporting are in place. The application remains responsible for pipeline creation, while `vk-bridge` provides reflection and layout helpers.

### CMake targets

| Target | Type | Dependencies | Availability |
| --- | --- | --- | --- |
| `vk-shell` | Static library | Vulkan | Always built |
| `spirv-reflect` | Static library | SPIRV-Reflect source tree | Always built |
| `vk-bridge` | Static library | Vulkan, SPIRV-Tools, `spirv-reflect` | Always built |
| `vk-toolkit-clear-color` | Executable | `vk-shell`, GLFW, X11, libm | `VK_TOOLKIT_BUILD_EXAMPLES=ON` |
| `vk-toolkit-scene` | Executable | `vk-shell`, `vk-bridge`, Slang, GLFW, X11, libm | `VK_TOOLKIT_BUILD_EXAMPLES=ON` |
| `vk-toolkit-compute` | Executable | `vk-shell`, `vk-bridge`, Slang, libm | `VK_TOOLKIT_BUILD_EXAMPLES=ON` |

The install exports the library targets with the `vkToolkit::` namespace.

## Examples

| Executable | Description |
| --- | --- |
| `vk-toolkit-clear-color` | Minimal C and GLFW example that creates a swapchain and clears its images with dynamic rendering. |
| `vk-toolkit-scene` | C++ graphics example using Slang shaders, shader reflection, indexed instanced drawing, an animated triangle, and multisampling. |
| `vk-toolkit-compute` | Slang compute example that uploads input data, dispatches a compute shader, reads the results back, and checks them on the CPU. |

When examples are enabled, shader sources are installed under `share/shaders` and the graphics examples require a working window system.

## Build Nix derivation

The flake provides the `vk-toolkit` package and a default development shell. The package derivation enables examples.

### Dev shell

You can build the project using a Nix dev shell:

```sh
nix develop .
mkdir build
cmake -S . -B ./build
cmake --build build
cmake --install build --prefix "$PWD/install"
./bin/install/vk-toolkit-clear-color
./bin/install/vk-toolkit-scene
./bin/install/vk-toolkit-compute
```


### Build

```sh
nix build .#vk-toolkit
./result/bin/vk-toolkit-compute
```

The GUI examples are installed alongside the compute example as `result/bin/vk-toolkit-clear-color` and `result/bin/vk-toolkit-scene`; they require a usable display session.

### Import Nix module

Import the package expression from a checkout or fetched source tree:

```nix
{ pkgs, ... }:
let
  vk-toolkit = import /path/to/vk-toolkit/nix { inherit pkgs; };
in
{
  environment.systemPackages = [ vk-toolkit ];
}
```

### Import as a flake

Add vk-toolkit as an input:

```nix
{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

    vk-toolkit = {
      url = "github:mrsinho/shvulkan";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };
}
```

Then refer to its package for the current system:

```nix
{ inputs, pkgs, ... }:
{
  environment.systemPackages = [
    inputs.vk-toolkit.packages.${pkgs.system}.default
  ];
}
```

## Build with CMake only

### Requirements

All the requirements are already provided by the [Nix flake](./flake.nix) in the repository:
- CMake 3.25 or newer.
- C and C++ compilers.
- Vulkan headers, loader, and development package.
- SPIRV-Tools.
- SPIRV-Reflect source files (`spirv_reflect.h` and `spirv_reflect.c`).
- For examples: Slang, GLFW, and X11 development files. The clear-color and scene examples also need a windowing environment at runtime.

The SPIRV-Reflect source directory must be provided through `VK_TOOLKIT_SPIRV_REFLECT_ROOT_DIR` as a CMake variable or environment variable.

```sh
cmake -S . -B build \
  -DVK_TOOLKIT_SPIRV_REFLECT_ROOT_DIR=/path/to/SPIRV-Reflect
cmake --build build
cmake --install build --prefix "$PWD/install"
```

The base build creates and installs the `vk-shell`, `spirv-reflect`, and `vk-bridge` static libraries. To include the examples, set `VK_TOOLKIT_BUILD_EXAMPLES=ON`:

After installing the package, a CMake consumer can use the exported library targets:

```cmake
find_package(vkToolkit CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE
  vkToolkit::vk-shell
  vkToolkit::vk-bridge
)
```

## Recommended Vulkan guides

- [Vulkan Tutorial (vklearn)](https://vklearn.org/)
- [Sascha Willems Vulkan samples](https://github.com/SaschaWillems/Vulkan)
- [Vulkan Guide](https://vkguide.dev/)

### Future shengine architecture

- Replace update-style callbacks (`update`, `graphicsUpdate`, and similar) with rendering organized around materials.
- Keep the engine responsible for instance, device, swapchain, and frames in flight.
- Let the application set the viewport position and size in its main loop for flexibility.
- Define application hooks for post-setup, before/during/after rendering, and before/after the main loop.
