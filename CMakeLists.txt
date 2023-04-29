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
    # "thirdparty/Vulkan-Headers/include/"
    "thirdparty/glfw/include/"
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    glfw 
    Vulkan::Vulkan
)

IF (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE dbghelp)
ENDIF()

find_package(Vulkan COMPONENTS glslc)
find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)

function(compile_shader target)
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "ENV" "SOURCES")
    foreach(source ${arg_SOURCES})
        add_custom_command(
            OUTPUT ${source}.spv
            DEPENDS ${source}
            DEPFILE ${source}.d
            COMMAND
                ${glslc_executable}
                $<$<BOOL:${arg_ENV}>:--target-env=${arg_ENV}>
                -MD -MF ${source}.d
                -Os
                -o ${source}.spv
                ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${source}
        )
        target_sources(${target} PRIVATE ${source}.spv)
    endforeach()
endfunction()

configure_file(
    "${PROJECT_SOURCE_DIR}/include/config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/config.h"
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders/triangle.frag
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders/triangle.vert
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

compile_shader(${PROJECT_NAME}
    ENV vulkan
    SOURCES
        triangle.frag
        triangle.vert
)
