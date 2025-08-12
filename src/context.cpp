
#include "context.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <spdlog/spdlog.h>

#include <stdexcept>
#include <string>

#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"
#include "constants.h"
#include "imgui.h"

AppContext::AppContext()
    : window(nullptr),
      renderer(nullptr),
      font(),
      target_frame_rate(TARGET_FRAME_RATE),
      io(nullptr) {
  if (!SDL_CreateWindowAndRenderer(APPLICATION_TITLE.c_str(), WINDOW_WIDTH,
                                   WINDOW_HEIGHT, WINDOW_FLAGS, &window,
                                   &renderer)) {
    SPDLOG_ERROR("Couldn't create window/renderer: {}", SDL_GetError());
    throw std::runtime_error("Failed to create window/renderer");
  }

  if (!TTF_Init()) {
    SPDLOG_ERROR("Couldn't initialize SDL_ttf: {}", SDL_GetError());
    throw std::runtime_error("Failed to initialize SDL_ttf");
  }

  font.loadFont();

  SPDLOG_INFO("Target frame rate set to {}", TARGET_FRAME_RATE);

  SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
  SDL_ShowWindow(window);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  io = &ImGui::GetIO();
  io->ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());

  ImGuiStyle& style = ImGui::GetStyle();
  style.ScaleAllSizes(main_scale);
  style.FontScaleDpi = main_scale;

  // Setup Platform/Renderer backends
  ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer3_Init(renderer);
}

AppContext::~AppContext() {
  if (renderer) {
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
  }

  if (window) {
    SDL_DestroyWindow(window);
    window = nullptr;
  }

  ImGui_ImplSDLRenderer3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();
}
