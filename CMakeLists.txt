cmake_minimum_required(VERSION 3.20)
project(slinger LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add SDL3 submodule
add_subdirectory(external/sdl)
add_subdirectory(external/sdl_ttf)

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/include
)

# Gather source files
file(GLOB_RECURSE SLINGER_SOURCES
    ${PROJECT_SOURCE_DIR}/src/*.cpp
)

# Create executable
add_executable(slinger ${SLINGER_SOURCES})

# Link SDL3
target_link_libraries(slinger PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf)

# Copy assets folder to the build directory
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})


# Optionally: if you want to use SDL_main (for Win/macOS entry point)
# target_link_libraries(slinger PRIVATE SDL3::SDL3main)

# If you want compiler warnings
target_compile_options(slinger PRIVATE -Wall -Wextra -Wpedantic)

