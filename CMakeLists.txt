cmake_minimum_required(VERSION 3.16)
project(SDL_Animations LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SDL_VENDORED ON)
set(SDLTTF_VENDORED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable verbose warnings
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Add SDL3 and SDL_ttf submodules
add_subdirectory(lib/SDL EXCLUDE_FROM_ALL)
add_subdirectory(lib/SDL_ttf EXCLUDE_FROM_ALL)
add_subdirectory(lib/spdlog EXCLUDE_FROM_ALL)

# Add include directories for your code and SDL headers
include_directories(
    src
    lib/SDL/include
    lib/SDL_ttf/include
    lib/spdlog/include
)

# Add your source files
set(SOURCES
    src/main.cpp
    src/context.cpp
    src/event_loop.cpp
    src/graphics/graphics.cpp
    src/graphics/fonts.cpp
    src/graphics/embedded_fonts/space_mono_regular.h
)

# Define the executable
add_executable(SDL_Animations ${SOURCES})

# Link SDL3 and SDL3_ttf libraries
target_link_libraries(
    SDL_Animations
    PRIVATE
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
    spdlog::spdlog
)
