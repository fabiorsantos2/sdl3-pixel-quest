cmake_minimum_required(VERSION 3.15)
project(MyGame VERSION 0.1.0 LANGUAGES CXX)

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Option to use vendored SDL3 (git submodule) vs system package
option(MYGAME_VENDORED "Use vendored SDL3" OFF)

if(MYGAME_VENDORED)
    # Assumes SDL source is in a git submodule at deps/SDL
    add_subdirectory(deps/SDL EXCLUDE_FROM_ALL)
else()
    find_package(SDL3 REQUIRED CONFIG)
    find_package(SDL3_image REQUIRED CONFIG)
endif()

# Source files
file(GLOB_RECURSE GAME_SOURCES "src/*.cpp" "src/*.h")

# Create the executable
add_executable(MyGame ${GAME_SOURCES})

# Link SDL3
target_link_libraries(MyGame PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
)

# Include directories (SDL3 headers are handled by the SDL3::SDL3 target)
target_include_directories(MyGame PRIVATE "${CMAKE_SOURCE_DIR}/src")

# Compiler warnings
if(MSVC)
    target_compile_options(MyGame PRIVATE /W4 /WX)
else()
    target_compile_options(MyGame PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Copy SDL3 DLL on Windows
if(WIN32 AND NOT MYGAME_VENDORED)
    add_custom_command(TARGET MyGame POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3>
        $<TARGET_FILE_DIR:MyGame>)
endif()

# Install target
install(TARGETS MyGame DESTINATION bin)