cmake_minimum_required(VERSION 3.13)
project(vuxol)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

find_package(Vulkan REQUIRED)
find_package(spdlog REQUIRED)

add_subdirectory(thirdparty/glfw)
add_subdirectory(thirdparty/vma)

find_program(MOLD_LINKER_EXE mold)

if(MOLD_LINKER_EXE)
    # If mold is found, set the linker flags to use it
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=mold")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=mold")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -fuse-ld=mold")
else()
    # If mold is not found, print a warning message and use the default linker
    message(WARNING "mold linker not found on this system. Using default linker instead.")
endif()
file(GLOB_RECURSE SOURCES 
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/src/*.h
    ${PROJECT_SOURCE_DIR}/include/*.h
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE 
    include
    ${CMAKE_CURRENT_BINARY_DIR}
    "thirdparty/glfw/include/"
    "thirdparty/vma/include"
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    glfw 
    VulkanMemoryAllocator
    spdlog::spdlog
    Vulkan::Vulkan
)

IF (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE dbghelp)
ENDIF()

find_package(Vulkan COMPONENTS glslc)

function(compile_shader target)
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "ENV" "SOURCES")
    foreach(source ${arg_SOURCES})
        add_custom_command(
            OUTPUT ${source}.spv
            DEPENDS ${source}
            DEPFILE ${source}.d
            COMMAND
                glslc
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
