cmake_minimum_required(VERSION 3.25...4.1.2)



if (DEFINED VK_TOOLKIT_SPIRV_REFLECT_ROOT_DIR)
    message(STATUS "vkToolkit: using CMake variable VK_TOOLKIT_SPIRV_REFLECT_ROOT_DIR")
elseif (DEFINED ENV{VK_TOOLKIT_SPIRV_REFLECT_ROOT_DIR})
    set(VK_TOOLKIT_SPIRV_REFLECT_ROOT_DIR "$ENV{VK_TOOLKIT_SPIRV_REFLECT_ROOT_DIR}")
    message(STATUS "vkToolkit: using environment variable VK_TOOLKIT_SPIRV_REFLECT_ROOT_DIR")
else()
    message(FATAL_ERROR"vkToolkit: missing VK_TOOLKIT_SPIRV_REFLECT_ROOT_DIR")
endif()



function(build_spirv_reflect)

add_library(spirv-reflect STATIC
    ${VK_TOOLKIT_SPIRV_REFLECT_ROOT_DIR}/spirv_reflect.h
    ${VK_TOOLKIT_SPIRV_REFLECT_ROOT_DIR}/spirv_reflect.c
)

target_include_directories(spirv-reflect PUBLIC
    ${VK_TOOLKIT_SPIRV_REFLECT_ROOT_DIR}
)

set_target_properties(spirv-reflect PROPERTIES PUBLIC_HEADER
    "${VK_TOOLKIT_SPIRV_REFLECT_ROOT_DIR}/spirv_reflect.h"
)

install(TARGETS spirv-reflect
    EXPORT vkToolkit
    PUBLIC_HEADER DESTINATION include/spirv-reflect
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

endfunction()