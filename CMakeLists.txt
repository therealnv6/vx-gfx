cmake_minimum_required(VERSION 3.13)
project(voxey)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

find_package(Vulkan REQUIRED)

add_subdirectory(thirdparty/glfw)

file(GLOB SOURCES 
    "src/*.cpp"
    "src/*.h" 
    "include/*.h"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE 
    include
    ${CMAKE_CURRENT_BINARY_DIR}
    "thirdparty/Vulkan-Headers/include/"
    "thirdparty/glfw/include/"
)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw Vulkan::Vulkan dbghelp)

add_custom_command(
    TARGET voxey
    POST_BUILD
    COMMAND
    ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/shaders ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}
)

configure_file(
    "${PROJECT_SOURCE_DIR}/include/config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/config.h"
)