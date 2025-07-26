# -------------------- Basic Setup --------------------
cmake_minimum_required(VERSION 3.15)                # Minimum required CMake version
project(FlosStellaEngine)                           # Project name

include(FetchContent)                               # Include module to fetch external content
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)               # Generate compile_commands.json (useful for clangd, LSPs)
set(CMAKE_CXX_STANDARD 17)                          # Use C++17

# Debug flags: disable optimizations, enable debug info
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")

# -------------------- Dependencies --------------------

# GLFW - windowing and input
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)

# GLM - header-only math library
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
)

# mINI - small INI parser
FetchContent_Declare(
    mini
    GIT_REPOSITORY https://github.com/pulzed/mINI.git
    GIT_TAG 0.9.18
)

# tinyobjloader - OBJ model loader
FetchContent_Declare(
    tinyobj
    GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader.git
    GIT_TAG v2.0.0rc13
)

# nlohmann/json - JSON for Modern C++
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
)

# Download and make available all the declared dependencies
FetchContent_MakeAvailable(glfw glm mini tinyobj nlohmann_json)

# -------------------- Environment & Platform Checks --------------------

# Check that Vulkan SDK is set
if(NOT DEFINED ENV{VULKAN_SDK})
    message(FATAL_ERROR "VULKAN_SDK environment variable not set!")
endif()

# Static link libstdc++/libgcc for MinGW builds (optional, avoids DLLs)
if(MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
endif()

# -------------------- Build Setup --------------------

# Collect all .cpp files in src/ recursively
file(GLOB_RECURSE SRC_FILES src/*.cpp)

# Define the executable target
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Include directories for this target
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include            # Your own project headers
    $ENV{VULKAN_SDK}/Include               # Vulkan headers
    ${mini_SOURCE_DIR}/src                 # mINI headers (not installed globally)
)

# Vulkan library directory
target_link_directories(${PROJECT_NAME} PRIVATE $ENV{VULKAN_SDK}/Lib)

# Link libraries to the executable
target_link_libraries(${PROJECT_NAME}
    vulkan-1                               # Vulkan runtime
    glfw                                   # GLFW window/input library
    glm::glm                               # GLM math library (interface target)
    tinyobjloader                          # tinyobjloader library
    nlohmann_json::nlohmann_json           # nlohmann_json library
)
