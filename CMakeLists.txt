cmake_minimum_required(VERSION 3.16)
project(SDL_Animations LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable verbose warnings
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Add SDL3 and SDL_ttf submodules
add_subdirectory(lib/SDL)
add_subdirectory(lib/SDL_ttf)

# Add include directories for your code and SDL headers
include_directories(
    src
    lib/SDL/include
    lib/SDL_ttf/include
)

# Add your source files
set(SOURCES
    src/main.cpp
    src/context.cpp
)

# Define the executable
add_executable(SDL_Animations ${SOURCES})

# Link SDL3 and SDL3_ttf libraries
target_link_libraries(SDL_Animations
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
)
