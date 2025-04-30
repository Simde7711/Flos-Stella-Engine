cmake_minimum_required(VERSION 3.15)
project(VulkanEngine)

set(CMAKE_CXX_STANDARD 17)

# Set path to Vulkan SDK (if not set via environment)
if(NOT DEFINED ENV{VULKAN_SDK})
    message(FATAL_ERROR "VULKAN_SDK environment variable not set!")
endif()

# Compiler flags for MinGW
if (MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
endif()

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/glfw-3.4.bin.WIN64/include
    ${CMAKE_SOURCE_DIR}/external/glm
    ${CMAKE_SOURCE_DIR}/external/tinyObjLoader
    $ENV{VULKAN_SDK}/Include
)

# Link directories (for Vulkan and glfw compiled libs)
link_directories(
    $ENV{VULKAN_SDK}/Lib
)

# Source files
file(GLOB_RECURSE SRC_FILES
    src/*.cpp
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

# Link GLFW and Vulkan (assuming pre-built GLFW static lib is in external/glfw/lib-mingw)
target_link_libraries(${PROJECT_NAME}
    ${CMAKE_SOURCE_DIR}/external/glfw-3.4.bin.WIN64/lib-mingw-w64/libglfw3.a
    vulkan-1
)