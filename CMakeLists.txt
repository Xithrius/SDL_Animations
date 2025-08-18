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

add_subdirectory(lib/SDL EXCLUDE_FROM_ALL)
add_subdirectory(lib/SDL_ttf EXCLUDE_FROM_ALL)
add_subdirectory(lib/spdlog EXCLUDE_FROM_ALL)

# Add include directories for your code and SDL headers
include_directories(
    src
    lib/SDL/include
    lib/SDL_ttf/include
    lib/spdlog/include
    lib/imgui
    lib/imgui/backends
)

# Add your source files
set(SOURCES
    src/main.cpp
    src/core/context.cpp
    src/core/app_state.cpp
    src/event_loop.cpp
    src/entities/entity.cpp
    src/entities/circle.cpp
    src/entities/line.cpp
    src/entities/point.cpp
    src/entities/rectangle.cpp
    src/entities/triangle.cpp
    src/entities/waypoint.cpp
    src/graphics/renderer.cpp
    src/graphics/fonts.cpp
    src/systems/input_system.cpp
    src/systems/animation_system.cpp
    src/ui/ui.cpp
    src/ui/debug.cpp
    src/graphics/embedded_fonts/space_mono_regular.h
    src/utils/uuid.cpp
    src/entities/isometric_cube.cpp
    src/ui/settings.cpp

    # ImGui core files
    lib/imgui/imgui.cpp
    lib/imgui/imgui_demo.cpp
    lib/imgui/imgui_draw.cpp
    lib/imgui/imgui_tables.cpp
    lib/imgui/imgui_widgets.cpp
    # ImGui SDL3 backend
    lib/imgui/backends/imgui_impl_sdl3.cpp
    lib/imgui/backends/imgui_impl_sdlrenderer3.cpp
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
