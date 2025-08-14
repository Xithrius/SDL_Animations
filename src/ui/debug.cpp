#include "debug.h"

#include "core/constants.h"
#include "entities/circle.h"
#include "entities/line.h"
#include "ui/ui.h"

void DebugUI::render() {
  if (!this->visible) return;

  ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
  ImGui::Begin("Settings", &this->visible);

  ImGui::SliderFloat("Point Speed Multiplier",
                     &getAppState()->point_speed_multiplier, 0.0f, 10.0f);
  ImGui::ColorEdit3("clear color", (float*)&CLEAR_COLOR);

  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
              1000.0f / getAppState()->io->Framerate,
              getAppState()->io->Framerate);

  ImGui::Text("Event Logic: %d Hz", static_cast<int>(TICK_RATE));

  ImGui::Separator();
  ImGui::Text("Entity System:");

  static bool newEntitySystemDemo = false;
  if (ImGui::Checkbox("New entity system demo", &newEntitySystemDemo)) {
    if (newEntitySystemDemo) {
      getUI()->createDemoEntities();
    } else {
      getAppState()->entityManager.clear();
    }
  }

  ImGui::Text("Entity Count: %zu",
              getAppState()->entityManager.getEntityCount());

  if (ImGui::Button("Add Random Line")) {
    int windowWidth, windowHeight;
    SDL_GetWindowSize(getAppState()->context->window, &windowWidth,
                      &windowHeight);

    auto* line = getAppState()->entityManager.createEntity<LineEntity>(
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

  if (ImGui::Button("Add Random Circle")) {
    int windowWidth, windowHeight;
    SDL_GetWindowSize(getAppState()->context->window, &windowWidth,
                      &windowHeight);

    auto* circle = getAppState()->entityManager.createEntity<CircleEntity>(
        SDL_FPoint{SDL_randf() * windowWidth, SDL_randf() * windowHeight},
        SDL_randf() * 50.0f + 10.0f);
    circle->setColor({static_cast<Uint8>(SDL_randf() * 255),
                      static_cast<Uint8>(SDL_randf() * 255),
                      static_cast<Uint8>(SDL_randf() * 255), 128});
    circle->setFilled(SDL_randf() > 0.5f);
  }

  if (ImGui::Button("Clear All Entities")) {
    getAppState()->entityManager.clear();
  }

  ImGui::End();
}
