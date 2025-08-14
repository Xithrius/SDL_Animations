#include "debug.h"

#include "entities/circle.h"
#include "entities/line.h"
#include "event_loop.h"
#include "imgui.h"
#include "ui/ui.h"

void DebugUI::debugOptions() {
  ImGui::Checkbox("Debug Frames", &this->debugFrames);
  if (this->debugFrames) {
  }

  ImGui::Separator();
  ImGui::Text("Framerate Controls:");

  float targetFPS = getUI()->getEventLoop()->getTargetFPS();

  if (ImGui::SliderFloat("Target FPS", &targetFPS, 0.0f, 240.0f, "%.0f")) {
    getUI()->getEventLoop()->setTargetFPS(targetFPS);
  }

  ImGui::Text("Current FPS: %.1f", this->getAppState()->io->Framerate);

  if (ImGui::Button("30 FPS")) {
    targetFPS = 30.0f;
    getUI()->getEventLoop()->setTargetFPS(targetFPS);
  }
  ImGui::SameLine();
  if (ImGui::Button("60 FPS")) {
    targetFPS = 60.0f;
    getUI()->getEventLoop()->setTargetFPS(targetFPS);
  }
  ImGui::SameLine();
  if (ImGui::Button("120 FPS")) {
    targetFPS = 120.0f;
    getUI()->getEventLoop()->setTargetFPS(targetFPS);
  }
  ImGui::SameLine();
  if (ImGui::Button("Unlimited")) {
    targetFPS = 0.0f;
    getUI()->getEventLoop()->setTargetFPS(targetFPS);
  }

  // Line rotation controls
  ImGui::Separator();
  ImGui::Text("Line Rotation Controls:");

  static float rotationSpeed = 1.0f;
  if (ImGui::SliderFloat("Rotation Speed", &rotationSpeed, 0.0f, 5.0f)) {
    // Update rotation speed for all line entities
    auto lines = getAppState()->entityManager.getEntitiesOfType<LineEntity>();
    for (auto* line : lines) {
      line->setRotationSpeed(rotationSpeed);
    }
  }

  if (ImGui::Button("Create Spinning Line")) {
    int windowWidth, windowHeight;
    SDL_GetWindowSize(getAppState()->context->window, &windowWidth,
                      &windowHeight);

    // Create a line at the center of the screen
    SDL_FPoint center = {static_cast<float>(windowWidth) / 2.0f,
                         static_cast<float>(windowHeight) / 2.0f};
    SDL_FPoint start = {center.x - 50.0f, center.y};
    SDL_FPoint end = {center.x + 50.0f, center.y};

    auto* line =
        getAppState()->entityManager.createEntity<LineEntity>(start, end);
    line->setRotationSpeed(rotationSpeed);
    line->setThickness(3.0f);

    // Set gradient properties for visual appeal
    LineEntity::GradientProperties gradientProps;
    gradientProps.enabled = true;
    gradientProps.startColor = {255, 0, 0, 255};  // Red
    gradientProps.endColor = {0, 0, 255, 255};    // Blue
    line->setGradientProperties(gradientProps);
  }
}

void DebugUI::render() {
  if (!this->visible) return;

  ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
  ImGui::Begin("Settings", &this->visible);

  this->debugOptions();

  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
              1000.0f / getAppState()->io->Framerate,
              getAppState()->io->Framerate);

  ImGui::Separator();
  ImGui::Text("Entity System:");

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
