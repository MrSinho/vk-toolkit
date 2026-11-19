cmake_minimum_required(VERSION 3.25...4.1.2)



if (DEFINED VK_TOOLKIT_ROOT_DIR)
    message(STATUS "vkToolkit: using CMake variable VK_TOOLKIT_ROOT_DIR")
elseif (DEFINED ENV{VK_TOOLKIT_ROOT_DIR})
    set(VK_TOOLKIT_ROOT_DIR "$ENV{VK_TOOLKIT_ROOT_DIR}")
    message(STATUS "vkToolkit: using environment variable VK_TOOLKIT_ROOT_DIR")
else()
    message(FATAL_ERROR "vkToolkit: missing VK_TOOLKIT_ROOT_DIR")
endif()



function(buildVkBridge)

add_library(vk-bridge STATIC
    ${VK_TOOLKIT_ROOT_DIR}/vk-bridge/src/vk-bridge.cpp
)

target_include_directories(vk-bridge PUBLIC
    $<BUILD_INTERFACE:${VK_TOOLKIT_ROOT_DIR}/vk-bridge/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(vk-bridge PUBLIC
    Vulkan::Vulkan
    SPIRV-Tools
    spirv-reflect
)

install(TARGETS vk-bridge
    EXPORT vkToolkit
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(
    DIRECTORY ${VK_TOOLKIT_ROOT_DIR}/vk-bridge/include/
    DESTINATION include
)

endfunction()
