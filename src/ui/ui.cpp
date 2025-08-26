#include "ui.h"

#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"
#include "core/app_state.h"
#include "entities/entity.h"

void UI::render() {
  // Initialize ImGui frame
  ImGui_ImplSDLRenderer3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();

  // Render UI components
  this->debug.render();
  this->settings.render();
  this->audioUI.render();

  // Render ImGui
  ImGui::Render();
  ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(),
                                        this->appState->context->renderer);
}
