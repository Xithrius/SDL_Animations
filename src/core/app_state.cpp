#include "app_state.h"

#include <spdlog/spdlog.h>

#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"
#include "constants.h"

AppState::AppState(Context* context) : context(context), entityManager(this) {
  font.loadFont();

  // Set up the renderer with the loaded font
  renderer.setRenderer(context->renderer);
  renderer.setFont(std::shared_ptr<TTF_Font>(
      font.get(),
      [](TTF_Font*) { /* Custom deleter to avoid double deletion */ }));

  spdlog::info("Target frame rate set to {}", TARGET_FRAME_RATE);

  SDL_SetWindowPosition(context->window, SDL_WINDOWPOS_CENTERED,
                        SDL_WINDOWPOS_CENTERED);
  SDL_ShowWindow(context->window);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  io = &ImGui::GetIO();
  io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  ImGui::StyleColorsDark();

  float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());

  ImGuiStyle& style = ImGui::GetStyle();
  style.ScaleAllSizes(main_scale);
  style.FontScaleDpi = main_scale;

  // Setup Platform/Renderer backends
  ImGui_ImplSDL3_InitForSDLRenderer(context->window, context->renderer);
  ImGui_ImplSDLRenderer3_Init(context->renderer);
}

AppState::~AppState() {
  ImGui_ImplSDLRenderer3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();
}
