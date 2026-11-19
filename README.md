# vk-toolkit

[![GitHub repository](https://img.shields.io/badge/GitHub-vk--toolkit-181717?style=for-the-badge&logo=github)](https://github.com/mrsinho/shvulkan)
![License](https://img.shields.io/github/license/mrsinho/shvulkan?style=for-the-badge)
![C and C++](https://img.shields.io/badge/C%20and%20C++-compatible-3CFADC?style=for-the-badge)

vk-toolkit is a small set of libraries and examples for building Vulkan applications. It provides `vk-shell` for common Vulkan setup and utility operations, and `vk-bridge` for shader reflection and Vulkan layout helpers. The application keeps control of Vulkan objects and pipeline creation.

The libraries expose C-friendly headers. `vk-shell` is implemented in C; `vk-bridge` is implemented in C++ and has a C-compatible API. The examples include both C and C++ code.

## Features

- Helpers for common Vulkan setup and API operations.
- Shader reflection using SPIRV-Reflect, with descriptor and push-constant merging through `vk-bridge`.
- Application control over Vulkan, Slang, and SPIRV-Reflect APIs.
- CMake and Nix build support.
- Vulkan dynamic rendering, dynamic state, and Synchronization 2 demonstrated in the examples.
- Examples for clear-color rendering, a reflected Slang scene, and compute dispatch.

The project has moved from the old `shvulkan` wrapper layout to separate `vk-shell` and `vk-bridge` libraries. It uses Slang for the example shaders and SPIRV-Reflect for shader metadata. `vk-bridge` supplies helpers for reflection merging and Vulkan layout creation; graphics and compute pipeline creation stays in the application.

## Examples

| Executable | Description |
| --- | --- |
| `vk-toolkit-clear-color` | Minimal C and GLFW example that creates a swapchain and clears its images with dynamic rendering. |
| `vk-toolkit-scene` | C++ graphics example using Slang shaders, shader reflection, indexed instanced drawing, an animated triangle, and multisampling. |
| `vk-toolkit-compute` | Slang compute example that uploads input data, dispatches a compute shader, reads the results back, and checks them on the CPU. |

When examples are enabled, shader sources are installed under `share/shaders` and the graphics examples require a working window system.

## Build with CMake

### Requirements

- CMake 3.25 or newer.
- A C and C++ compiler.
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

```sh
cmake -S . -B build \
  -DVK_TOOLKIT_SPIRV_REFLECT_ROOT_DIR=/path/to/SPIRV-Reflect \
  -DVK_TOOLKIT_BUILD_EXAMPLES=ON
cmake --build build
```

After installing the package, a CMake consumer can use the exported library targets:

```cmake
find_package(vkToolkit CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE
  vkToolkit::vk-shell
  vkToolkit::vk-bridge
)
```

### CMake targets and options

| Target | Type | Availability |
| --- | --- | --- |
| `vk-shell` | Static library | Always built |
| `spirv-reflect` | Static library | Always built |
| `vk-bridge` | Static library | Always built |
| `vk-toolkit-clear-color` | Executable | `VK_TOOLKIT_BUILD_EXAMPLES=ON` |
| `vk-toolkit-scene` | Executable | `VK_TOOLKIT_BUILD_EXAMPLES=ON` |
| `vk-toolkit-compute` | Executable | `VK_TOOLKIT_BUILD_EXAMPLES=ON` |

The install exports the library targets with the `vkToolkit::` namespace.

| Option or variable | Default | Purpose |
| --- | --- | --- |
| `VK_TOOLKIT_BUILD_EXAMPLES` | `OFF` | Build and install the three example executables and shader sources. |
| `VK_TOOLKIT_BUILD_DOCS` | `OFF` | Reserved for documentation generation. The documentation sources and CMake integration are currently absent, so enabling this option is not supported yet. |
| `VK_TOOLKIT_SPIRV_REFLECT_ROOT_DIR` | Required | Path to the SPIRV-Reflect source checkout. Can also be set in the environment. |
| `VK_TOOLKIT_ROOT_DIR` | Set by the project | Root path used by the included CMake scripts. |
| `VK_TOOLKIT_VERSION` | `1.1.1` | Project/package version. |

`VK_TOOLKIT_BUILD_EXAMPLES` and `VK_TOOLKIT_BUILD_DOCS` can also be set as environment variables to `ON`. The documentation option is declared but its build function is not currently connected.

## Nix

The flake provides the `vk-toolkit` package and a default development shell. The package derivation enables examples.

### Development shell

```sh
nix develop .
cmake -S . -B build -G Ninja
cmake --build build
```

The development shell supplies the Vulkan, Slang, SPIRV-Tools, SPIRV-Reflect, GLFW, and window-system dependencies. It also enables example configuration by default.

### Build and run the package

```sh
nix build .#vk-toolkit
./result/bin/vk-toolkit-compute
```

The GUI examples are installed alongside the compute example as `result/bin/vk-toolkit-clear-color` and `result/bin/vk-toolkit-scene`; they require a usable display session.

### Use the package from a NixOS configuration

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

### Use the package from another flake

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

## Recommended Vulkan guides

- [Vulkan Tutorial (vklearn)](https://vklearn.org/)
- [Sascha Willems Vulkan samples](https://github.com/SaschaWillems/Vulkan)
- [Vulkan Guide](https://vkguide.dev/)

## Roadmap

- Android build using the same codebase (Android NDK).
- Web build using the same codebase.
- Rename the repository to `vk-toolkit` and the KDevelop project file to `vk-toolkit.kdev4`.
- Improve vertex-input reflection: flatten composite types, support struct inputs, and use reflected location and binding data to determine input layout.
- Add a tessellation state example.
- Add a geometry shader example.
- Add a PBR shader example.
- Test stencil attachments.
- Add a model-inference example.
- Consolidate the reusable example utilities under a clearly named common layer and document why each helper exists.

The Nix and CMake build path, Slang shader compilation in the examples, the CMake package export, and application-controlled error reporting are in place. The application remains responsible for pipeline creation, while `vk-bridge` provides reflection and layout helpers.

### Shader reflection through SPIRV-Reflect and vk-bridge

- [~] Entry points.
- [x] Descriptor sets.
- [x] Push-constant ranges.
- [x] Vertex inputs; composite struct inputs still need support. Future work includes looking up locations by name and using binding metadata to determine offsets.
- [x] Specialization constants; looking up constant IDs by name remains future work.
- [ ] Atomics, demonstrated with a quad whose fragments follow a sequence without interpolation, or with a simpler compute example.
- [ ] Textures.
- [ ] Dispatch local size.

### Future shengine architecture

- Replace update-style callbacks (`update`, `graphicsUpdate`, and similar) with rendering organized around materials.
- Keep the engine responsible for instance, device, swapchain, and frames in flight.
- Let the application set the viewport position and size in its main loop for flexibility.
- Define application hooks for post-setup, before/during/after rendering, and before/after the main loop.
