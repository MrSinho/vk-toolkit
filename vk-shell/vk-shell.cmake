cmake_minimum_required(VERSION 3.25...4.1.2)



if (DEFINED VK_TOOLKIT_ROOT_DIR)
    message(STATUS "vkToolkit: using CMake variable VK_TOOLKIT_ROOT_DIR")
elseif (DEFINED ENV{VK_TOOLKIT_ROOT_DIR})
    set(VK_TOOLKIT_ROOT_DIR "$ENV{VK_TOOLKIT_ROOT_DIR}")
    message(STATUS "vkToolkit: using environment variable VK_TOOLKIT_ROOT_DIR")
else()
    message(FATAL_ERROR "vkToolkit: missing VK_TOOLKIT_ROOT_DIR")
endif()



function(buildVkShell)

find_package(Vulkan REQUIRED)

add_library(vk-shell STATIC
    "${VK_TOOLKIT_ROOT_DIR}/vk-shell/src/vk-shell/vk-shell.c"
)

target_include_directories(vk-shell PUBLIC
    "$<BUILD_INTERFACE:${VK_TOOLKIT_ROOT_DIR}/vk-shell/include>"
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(vk-shell PUBLIC
    Vulkan::Vulkan
)

install(TARGETS vk-shell
    EXPORT vkToolkit
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(
    DIRECTORY "${VK_TOOLKIT_ROOT_DIR}/vk-shell/include/"
    DESTINATION include
)

endfunction()
