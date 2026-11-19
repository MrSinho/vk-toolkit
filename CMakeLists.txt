cmake_minimum_required(VERSION 3.25...4.1.2)



set(VK_TOOLKIT_VERSION  1.1.1)
set(VK_TOOLKIT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(VK_TOOLKIT_REPO_URL "https://github.com/mrsinho/shvulkan")
set(VK_TOOLKIT_DOCS_URL "https://github.com/mrsinho/shvulkan")
set(VK_TOOLKIT_DOCS_DIR ${VK_TOOLKIT_ROOT_DIR}/docs CACHE PATH "vkToolkit documentation directory")
set(VK_TOOLKIT_SHVULKAN_OUT_DOCS_DIR ${VK_TOOLKIT_DOCS_DIR} CACHE PATH "Legacy documentation directory variable")



project(vkToolkit LANGUAGES C CXX)



include(${VK_TOOLKIT_ROOT_DIR}/vk-shell/vk-shell.cmake)
include(${VK_TOOLKIT_ROOT_DIR}/spirv-reflect/spirv-reflect.cmake)
include(${VK_TOOLKIT_ROOT_DIR}/vk-bridge/vk-bridge.cmake)
#include(${VK_TOOLKIT_ROOT_DIR}/docs/shvulkan-docs.cmake) #TODO
include(${VK_TOOLKIT_ROOT_DIR}/examples/examples.cmake)



option(VK_TOOLKIT_BUILD_DOCS "Build vkToolkit documentation" OFF)
option(VK_TOOLKIT_BUILD_EXAMPLES "Build vkToolkit examples" OFF)



find_package(Vulkan REQUIRED)
message(STATUS "vkToolkitMessage: found Vulkan")

find_package(SPIRV-Tools REQUIRED)
message(STATUS "vkToolkitMessage: found SPIRV-Tools")



function(main)

buildVkShell()

build_spirv_reflect()

buildVkBridge()

if (VK_TOOLKIT_BUILD_DOCS OR "$ENV{VK_TOOLKIT_BUILD_DOCS}" STREQUAL "ON")
build_shvulkan_docs()
endif()

if (VK_TOOLKIT_BUILD_EXAMPLES OR "$ENV{VK_TOOLKIT_BUILD_EXAMPLES}" STREQUAL "ON")
buildVkToolkitExamples()
endif()

install(
    EXPORT vkToolkit
    FILE vkToolkitTargets.cmake
    NAMESPACE vkToolkit::
    DESTINATION lib/cmake/vkToolkit
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${VK_TOOLKIT_ROOT_DIR}/cmake/vkToolkitConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/vkToolkitConfig.cmake
    INSTALL_DESTINATION lib/cmake/vkToolkit
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/vkToolkitConfigVersion.cmake
    VERSION ${VK_TOOLKIT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/vkToolkitConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/vkToolkitConfigVersion.cmake
    DESTINATION lib/cmake/vkToolkit
)

endfunction()



main()
