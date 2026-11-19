cmake_minimum_required(VERSION 3.25...4.1.2)

include(GNUInstallDirs)



if (DEFINED VK_TOOLKIT_ROOT_DIR)
    message(STATUS "vkToolkit: using CMake variable VK_TOOLKIT_ROOT_DIR")
elseif (DEFINED ENV{VK_TOOLKIT_ROOT_DIR})
    set(VK_TOOLKIT_ROOT_DIR "$ENV{VK_TOOLKIT_ROOT_DIR}")
    message(STATUS "vkToolkit: using environment variable VK_TOOLKIT_ROOT_DIR")
else()
    message(FATAL_ERROR"vkToolkit: missing VK_TOOLKIT_ROOT_DIR")
endif()



function(buildVkToolkitExamples)

find_package(slang REQUIRED)
message(STATUS "vkToolkitMessage: found Slang")

#if (DEFINED VK_TOOLKIT_GLFW_CMAKE_CONFIG_PATH) # Nix build
#include(${VK_TOOLKIT_GLFW_CMAKE_CONFIG_PATH})
##add_subdirectory(${VK_TOOLKIT_ROOT_DIR}/examples/external/glfw)
#endif()

add_executable(vk-toolkit-compute
    ${VK_TOOLKIT_ROOT_DIR}/examples/src/compute/main.cpp
    ${VK_TOOLKIT_ROOT_DIR}/examples/src/compute/init.cpp
    ${VK_TOOLKIT_ROOT_DIR}/examples/src/compute/cleanup.cpp
    ${VK_TOOLKIT_ROOT_DIR}/examples/src/compute/memory.cpp
    ${VK_TOOLKIT_ROOT_DIR}/examples/src/compute/pipeline.cpp
    ${VK_TOOLKIT_ROOT_DIR}/examples/src/compute/descriptors.cpp
)
add_executable(vk-toolkit-clear-color
    ${VK_TOOLKIT_ROOT_DIR}/examples/src/clear-color/main.c
    ${VK_TOOLKIT_ROOT_DIR}/examples/src/clear-color/swapchain.c
    ${VK_TOOLKIT_ROOT_DIR}/examples/src/clear-color/rendering.c
    ${VK_TOOLKIT_ROOT_DIR}/examples/src/clear-color/cleanup.c
)
add_executable(vk-toolkit-scene
    ${VK_TOOLKIT_ROOT_DIR}/examples/src/scene/main.cpp
    ${VK_TOOLKIT_ROOT_DIR}/examples/src/scene/swapchain.cpp
    ${VK_TOOLKIT_ROOT_DIR}/examples/src/scene/cleanup.cpp
    ${VK_TOOLKIT_ROOT_DIR}/examples/src/scene/data.cpp
    ${VK_TOOLKIT_ROOT_DIR}/examples/src/scene/memory.cpp
    ${VK_TOOLKIT_ROOT_DIR}/examples/src/scene/pipeline.cpp
    ${VK_TOOLKIT_ROOT_DIR}/examples/src/scene/descriptors.cpp
    ${VK_TOOLKIT_ROOT_DIR}/examples/src/scene/rendering.cpp
)
if (NOT TARGET vk-shell)
include(${VK_TOOLKIT_ROOT_DIR}/vk-shell/vk-shell.cmake)
endif()

if (NOT TARGET vk-bridge)
include(${VK_TOOLKIT_ROOT_DIR}/vk-bridge/vk-bridge.cmake)
endif()

target_link_libraries(vk-toolkit-clear-color PRIVATE vk-shell glfw X11 m)
target_link_libraries(vk-toolkit-scene       PRIVATE vk-shell vk-bridge slang glfw X11 m)
target_link_libraries(vk-toolkit-compute     PRIVATE vk-shell vk-bridge slang m)

target_compile_definitions(vk-toolkit-clear-color PRIVATE GRAPHICS_EXAMPLE)
target_compile_definitions(vk-toolkit-scene PRIVATE
    GRAPHICS_EXAMPLE
    VK_TOOLKIT_INSTALL_DATADIR="${VK_TOOLKIT_INSTALL_DATADIR}"
)
target_compile_definitions(vk-toolkit-compute PRIVATE
    VK_TOOLKIT_INSTALL_DATADIR="${VK_TOOLKIT_INSTALL_DATADIR}"
)

install(TARGETS vk-toolkit-compute vk-toolkit-clear-color vk-toolkit-scene
    EXPORT vkToolkit
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(
    DIRECTORY ${VK_TOOLKIT_ROOT_DIR}/examples/share/shaders/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/shaders
)

endfunction()
