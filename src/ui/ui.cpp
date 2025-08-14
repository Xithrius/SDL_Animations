#include "ui.h"

#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"
#include "core/app_state.h"
#include "entities/circle.h"
#include "entities/entity.h"
#include "entities/line.h"

void UI::render() {
  // Clear the renderer at the start of each frame
  SDL_SetRenderScale(this->appState->context->renderer,
                     this->appState->io->DisplayFramebufferScale.x,
                     this->appState->io->DisplayFramebufferScale.y);
  SDL_SetRenderDrawColorFloat(this->appState->context->renderer, CLEAR_COLOR.x,
                              CLEAR_COLOR.y, CLEAR_COLOR.z, CLEAR_COLOR.w);
  SDL_RenderClear(this->appState->context->renderer);

  ImGui_ImplSDLRenderer3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();

  this->debug.render();

  // Render entities
  this->appState->entityManager.render(this->appState->context->renderer);

  // Render ImGui
  ImGui::Render();
  ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(),
                                        this->appState->context->renderer);

  SDL_RenderPresent(this->appState->context->renderer);
}

void UI::createDemoEntities() {
  int windowWidth, windowHeight;
  SDL_GetWindowSize(this->appState->context->window, &windowWidth,
                    &windowHeight);

  // Create some demo circles
  for (int i = 0; i < 5; ++i) {
    auto* circle = this->appState->entityManager.createEntity<CircleEntity>(
        SDL_FPoint{SDL_randf() * windowWidth, SDL_randf() * windowHeight},
        SDL_randf() * 30.0f + 10.0f);
    circle->setColor({static_cast<Uint8>(SDL_randf() * 255),
                      static_cast<Uint8>(SDL_randf() * 255),
                      static_cast<Uint8>(SDL_randf() * 255), 128});
    circle->setFilled(SDL_randf() > 0.5f);
  }

  // Create some demo lines
  for (int i = 0; i < 3; ++i) {
    auto* line = this->appState->entityManager.createEntity<LineEntity>(
        SDL_FPoint{SDL_randf() * windowWidth, SDL_randf() * windowHeight},
        SDL_FPoint{SDL_randf() * windowWidth, SDL_randf() * windowHeight});

    LineEntity::GradientProperties gradientProps;
    gradientProps.enabled = true;
    gradientProps.startColor = {static_cast<Uint8>(SDL_randf() * 255),
                                static_cast<Uint8>(SDL_randf() * 255),
                                static_cast<Uint8>(SDL_randf() * 255), 255};
    gradientProps.endColor = {static_cast<Uint8>(SDL_randf() * 255),
                              static_cast<Uint8>(SDL_randf() * 255),
                              static_cast<Uint8>(SDL_randf() * 255), 255};
    line->setGradientProperties(gradientProps);
  }
}
